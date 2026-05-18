#include "Game.h"
#include "ConsoleUtils.h"
#include "items/UsableItem.h"
#include <iostream>
#include <random>

namespace {
const std::string kSep  =
    "================================================================================";
const std::string kLine =
    "--------------------------------------------------------------------------------";
std::mt19937 s_game_rng{ std::random_device{}() };
}

Game::Game(const std::string& data_dir) : data_dir_(data_dir) {}

void Game::Run() {
  LoadData();
  InitHero();
  ShowIntro();

  if (game_over_) return;

  PlayRoadSegment("road_to_castle1", "road_to_castle1_text");
  if (game_over_) return;

  RunCastle(1);
  if (game_over_) return;
  if (skip_to_ryota_) { HandleFinalApproach(); return; }

  PlayRoadSegment("road_to_castle2", "road_to_castle2_text");
  if (game_over_) return;

  RunCastle(2);
  if (game_over_) return;
  if (skip_to_ryota_) { HandleFinalApproach(); return; }

  PlayRoadSegment("road_to_castle3", "road_to_castle3_text");
  if (game_over_) return;

  RunCastle(3);
  if (game_over_) return;

  HandleFinalApproach();
}

void Game::LoadData() {
  db_.LoadAll(data_dir_);
}

void Game::InitHero() {
  int char_id = db_.GetConfigInt("starting_character_id", 1);
  const CharacterData* cd = db_.GetCharacterById(char_id);
  if (!cd) {
    std::cout << "[ОШИБКА] Данные героя не найдены (id=" << char_id << ").\n";
    game_over_ = true;
    return;
  }

  BodyParts parts{ cd->head_hp, cd->body_hp, cd->arms_hp, cd->legs_hp };
  int arms_pen = db_.GetConfigInt("broken_arms_attack_penalty", 3);
  int legs_pen = db_.GetConfigInt("broken_legs_agility_penalty", 3);
  hero_ = std::make_unique<Hero>(cd->name, cd->attack, cd->defense, cd->agility, parts, arms_pen, legs_pen);
  hero_->AddGold(cd->starting_gold);

  int inv_size = db_.GetConfigInt("max_inventory_size", 12);
  inventory_ = std::make_unique<Inventory>(inv_size);
  hero_->SetInventory(inventory_.get());

  battle_system_   = std::make_unique<BattleSystem>(db_);
  dialogue_system_ = std::make_unique<DialogueSystem>(db_);
}

void Game::ShowIntro() {
  ClearScreen();
  std::cout << "\n" << kSep << "\n";
  std::cout << "                         ЛИСТ САКУРЫ\n";
  std::cout << "                   Текстовая RPG в феодальной Японии\n";
  std::cout << kSep << "\n\n";
  Print(db_.GetConfig("intro_text"));
  std::cout << "\n";
  PressEnter();
}

void Game::PlayRoadSegment(const std::string& location, const std::string& road_text_key) {
  ClearScreen();
  PrintSep();
  Print(db_.GetConfig(road_text_key));
  std::cout << "\n";
  PressEnter();

  TriggerLocationEvent(location);
}

void Game::TriggerLocationEvent(const std::string& location) {
  const MerchantData* md = db_.GetMerchantByLocation(location);
  if (md) RunMerchantEncounter(*md);

  if (game_over_) return;

  const EventData* ev = db_.GetEventByLocation(location);
  if (!ev) return;

  std::vector<DialogueEffect> effects = dialogue_system_->StartDialogue(ev->start_dialogue_id);

  for (const DialogueEffect& effect : effects) {
    if (effect.type == "start_battle") {
      int enemy_id = std::stoi(effect.value);

      const EnemyData* ed = db_.GetEnemyById(enemy_id);
      if (!ed) return;

      BodyParts ep{ ed->head_hp, ed->body_hp, ed->arms_hp, ed->legs_hp };
      int arms_pen = db_.GetConfigInt("broken_arms_attack_penalty", 3);
      int legs_pen = db_.GetConfigInt("broken_legs_agility_penalty", 3);
      Enemy enemy(ed->name, ed->attack, ed->defense, ed->agility, ep,
                  ed->reward_gold, ed->description, arms_pen, legs_pen);

      bool won = battle_system_->StartBattle(*hero_, enemy);
      if (!won) { game_over_ = true; ShowDefeat(); }
    } else {
      ApplyDialogueEffect(effect.type, effect.value);
    }
  }
}

void Game::RunCastle(int castle_id) {
  const CastleData* cd = db_.GetCastleById(castle_id);
  if (!cd) return;

  ClearScreen();
  PrintSep();
  std::cout << "\n  ★ " << cd->name << " ★\n\n";
  Print(cd->intro_text);
  std::cout << "\n";
  PressEnter();

  auto room_datas = db_.GetRoomsForCastle(castle_id);
  std::vector<Room> rooms;
  rooms.reserve(room_datas.size());
  for (auto& rd : room_datas)
    rooms.emplace_back(rd);

  Castle castle(*cd, std::move(rooms));

  for (auto& room : castle.GetRooms()) {
    if (game_over_ || skip_to_ryota_) return;
    RunRoom(room);
  }

  if (finish_castle_then_ryota_) {
    skip_to_ryota_            = true;
    finish_castle_then_ryota_ = false;
  }

  if (!game_over_) castle.MarkCleared();
}

void Game::RunRoom(Room& room) {
  const RoomData& rd = room.GetData();

  ClearScreen();
  PrintLine();
  std::cout << "  Комната " << rd.room_number << ": ";
  Print(rd.description);
  std::cout << "\n";

  if (rd.enemy_id > 0 && !room.IsEnemyDefeated()) {
    const EnemyData* ed = db_.GetEnemyById(rd.enemy_id);
    if (ed) {
      PressEnter();
      BodyParts ep{ ed->head_hp, ed->body_hp, ed->arms_hp, ed->legs_hp };
      int arms_pen = db_.GetConfigInt("broken_arms_attack_penalty", 3);
      int legs_pen = db_.GetConfigInt("broken_legs_agility_penalty", 3);
      Enemy enemy(ed->name, ed->attack, ed->defense, ed->agility, ep,
                  ed->reward_gold, ed->description, arms_pen, legs_pen);

      bool won = battle_system_->StartBattle(*hero_, enemy);
      if (!won) {
        game_over_ = true;
        ShowDefeat();
        return;
      }
      room.MarkEnemyDefeated();
    }
  }

  if (!room.IsSearched() && rd.search_chance > 0)
    RunRoomSearch(room);
}

void Game::RunRoomSearch(Room& room) {
  const RoomData& rd = room.GetData();

  std::cout << "\n  Осмотреть комнату? (1=да  2=нет)\n";
  int choice = ReadInt(1, 2);
  if (choice == 2) return;

  room.MarkSearched();

  ClearScreen();
  PrintLine();
  std::cout << "\n  ★ ОБЫСК ★\n\n";

  bool found_something = false;

  std::uniform_int_distribution<int> dist(1, 100);
  bool hint_lucky = dist(s_game_rng) <= rd.search_chance;
  bool item_lucky = dist(s_game_rng) <= rd.search_chance;

  if (hint_lucky && rd.has_hint && !rd.hint_text.empty()) {
    ++hints_found_;
    std::cout << "  [Подсказка " << hints_found_ << "]: " << rd.hint_text << "\n";
    found_something = true;
  }

  if (item_lucky && rd.item_id > 0 && !rd.item_type.empty() && !room.IsItemCollected()) {
    GiveItemToHero(rd.item_type + ":" + std::to_string(rd.item_id));
    room.MarkItemCollected();
    found_something = true;
  }

  if (!found_something)
    std::cout << "  Ничего полезного не обнаружено.\n";

  std::cout << "\n";
  PressEnter();

  if (rd.has_hint && !rd.hint_text.empty()) {
    int threshold = db_.GetConfigInt("hints_for_shortcut", 3);
    if (hints_found_ >= threshold && !skip_to_ryota_
                                  && !finish_castle_then_ryota_
                                  && !shortcut_offered_)
      OfferShortcutToRyota();
  }
}

void Game::OfferShortcutToRyota() {
  shortcut_offered_ = true;

  ClearScreen();
  PrintSep();
  std::cout << "\n  ★ ПУТЬ ОТКРЫТ ★\n\n";
  Print(db_.GetConfig("hint_shortcut_text"));
  std::cout << "\n";
  std::cout << "  1. Идти к Рёте прямо сейчас\n";
  std::cout << "  2. Дочистить этот замок и идти сразу после\n";
  int choice = ReadInt(1, 2);

  flags_["knows_ryota"] = true;

  if (choice == 1) {
    skip_to_ryota_ = true;
  } else {
    finish_castle_then_ryota_ = true;
  }
}

void Game::HandleFinalApproach() {
  const MerchantData* final_md = db_.GetMerchantByLocation("road_to_ryota");
  if (final_md) RunMerchantEncounter(*final_md);
  if (game_over_) return;

  ClearScreen();
  PrintSep();
  Print(db_.GetConfig("road_to_ryota_text"));
  std::cout << "\n";

  if (flags_["knows_ryota"])
    Print(db_.GetConfig("ryota_knows_flag_text"));
  else
    Print(db_.GetConfig("ryota_unknown_text"));

  std::cout << "\n";
  PressEnter();

  const EventData* ev = db_.GetEventByLocation("pre_final_battle");
  if (!ev) return;

  std::vector<DialogueEffect> effects = dialogue_system_->StartDialogue(ev->start_dialogue_id);

  for (const DialogueEffect& effect : effects) {
    if (effect.type == "start_battle") {
      int enemy_id = std::stoi(effect.value);

      const EnemyData* ed = db_.GetEnemyById(enemy_id);
      if (!ed) return;

      BodyParts ep{ ed->head_hp, ed->body_hp, ed->arms_hp, ed->legs_hp };
      int arms_pen = db_.GetConfigInt("broken_arms_attack_penalty", 3);
      int legs_pen = db_.GetConfigInt("broken_legs_agility_penalty", 3);
      Enemy ryota(ed->name, ed->attack, ed->defense, ed->agility, ep,
                  ed->reward_gold, ed->description, arms_pen, legs_pen);

      bool won = battle_system_->StartBattle(*hero_, ryota);
      if (won) ShowVictory();
      else     ShowDefeat();
    } else {
      ApplyDialogueEffect(effect.type, effect.value);
    }
  }
}

void Game::ShowVictory() {
  std::cout << "\n" << kSep << "\n";
  std::cout << "                           ПОБЕДА\n";
  std::cout << kSep << "\n\n";
  Print(db_.GetConfig("victory_text"));
  std::cout << "\n" << kSep << "\n";
}

void Game::ShowDefeat() {
  std::cout << "\n" << kSep << "\n";
  std::cout << "                          ПОРАЖЕНИЕ\n";
  std::cout << kSep << "\n\n";
  Print(db_.GetConfig("defeat_text"));
  std::cout << "\n" << kSep << "\n";
}

void Game::RunMerchantEncounter(const MerchantData& md) {
  Merchant merchant = BuildMerchant(md);
  const auto& entries = merchant.GetShop().GetEntries();

  std::string notice;

  auto drawScreen = [&]() {
    ClearScreen();
    std::cout << kSep << "\n";
    std::cout << "  ТОРГОВЕЦ: " << merchant.GetName()
              << "   Золото: " << hero_->GetGold() << "\n";
    std::cout << kSep << "\n";

    std::cout << "\n  ИНВЕНТАРЬ ("
              << inventory_->Size() << "/"
              << db_.GetConfigInt("max_inventory_size", 12) << "):\n";
    const auto& inv_items = inventory_->GetItems();
    if (inv_items.empty()) {
      std::cout << "    (пусто)\n";
    } else {
      for (int i = 0; i < (int)(inv_items.size()); ++i)
        std::cout << "    " << (i + 1) << ". "
                  << inv_items[i]->GetName() << "\n";
    }

    std::cout << "\n" << kLine << "\n";
    std::cout << "  ТОВАРЫ:\n\n";
    if (entries.empty()) {
      std::cout << "    (нет товаров)\n";
    } else {
      for (int i = 0; i < (int)(entries.size()); ++i) {
        const auto& e = entries[i];
        std::cout << "    " << (i + 1) << ". " << e.item->GetName()
                  << "  —  " << e.price << " зол.\n";
        std::string desc = e.item->GetDescription();
        if (!desc.empty())
          std::cout << "       " << desc << "\n";
        std::cout << "\n";
      }
    }

    std::cout << kLine << "\n";
    if (!notice.empty()) {
      std::cout << "  " << notice << "\n";
      notice.clear();
    }
  };

  while (true) {
    drawScreen();
    if (entries.empty()) { PressEnter(); return; }

    std::cout << "  Купить (0 — уйти):\n";
    int choice = ReadInt(0, (int)(entries.size()));
    if (choice == 0) break;

    int price = entries[choice - 1].price;
    if (hero_->GetGold() < price) {
      notice = "Недостаточно золота.";
    } else if (inventory_->IsFull()) {
      notice = "Инвентарь полон!";
    } else {
      inventory_->AddItem(entries[choice - 1].item);
      hero_->SpendGold(price);
      notice = "Куплено: " + entries[choice - 1].item->GetName()
             + "  (осталось золота: " + std::to_string(hero_->GetGold()) + ")";
    }
  }
}

Merchant Game::BuildMerchant(const MerchantData& md) const {
  Shop shop;
  auto shop_items = db_.GetShopItems(md.shop_id);

  for (const auto& si : shop_items) {
    std::shared_ptr<Item> item;
    int price = si.price_override;

    if (si.item_type == "usable") {
      const UsableItemData* ud = db_.GetUsableItemById(si.item_id);
      if (!ud) continue;
      if (price == 0) price = ud->price;
      item = std::make_shared<UsableItem>(*ud);
    }

    if (item) shop.AddEntry({ std::move(item), price });
  }

  return Merchant(md.name, md.location, std::move(shop));
}

void Game::ApplyDialogueEffect(const std::string& effect_type,
                                const std::string& effect_value) {
  if (effect_type == "give_item") {
    GiveItemToHero(effect_value);
  } else if (effect_type == "set_flag") {
    auto eq = effect_value.find('=');
    if (eq != std::string::npos) {
      std::string flag_name  = effect_value.substr(0, eq);
      std::string flag_value = effect_value.substr(eq + 1);
      flags_[flag_name] = (flag_value == "true" || flag_value == "1");
    } else {
      flags_[effect_value] = true;
    }
  }
}

void Game::GiveItemToHero(const std::string& item_spec) {
  auto colon = item_spec.find(':');
  if (colon == std::string::npos) return;

  std::string item_type = item_spec.substr(0, colon);
  int item_id = std::stoi(item_spec.substr(colon + 1));

  std::shared_ptr<Item> item;

  if (item_type == "usable") {
    const UsableItemData* ud = db_.GetUsableItemById(item_id);
    if (ud) item = std::make_shared<UsableItem>(*ud);
  }

  if (!item) return;

  if (inventory_->AddItem(item)) {
    std::cout << "\n  Получен предмет: " << item->GetName() << "\n";
    std::string desc = item->GetDescription();
    if (!desc.empty()) Print(desc);
  } else {
    std::cout << "\n  Инвентарь полон — предмет не удалось взять.\n";
  }
}

void Game::Print(const std::string& text) const {
  if (!text.empty()) std::cout << "  " << text << "\n";
}

void Game::PrintSep() const {
  std::cout << "\n" << kSep << "\n";
}

void Game::PrintLine() const {
  std::cout << "\n" << kLine << "\n";
}

void Game::PressEnter() const {
  std::cout << "  [Нажми Enter для продолжения...]\n";
  std::cin.ignore(10000, '\n');
}

int Game::ReadInt(int min_val, int max_val) const {
  int value;
  while (true) {
    std::cout << "  > ";
    if (std::cin >> value && value >= min_val && value <= max_val)
      break;
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cout << "  Введи число от " << min_val << " до " << max_val << ".\n";
  }
  std::cin.ignore(10000, '\n');
  return value;
}
