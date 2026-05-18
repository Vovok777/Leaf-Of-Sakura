#include "GameDatabase.h"
#include "CsvParser.h"

namespace {

int toInt(const std::string& s, int def = 0) {
  if (s.empty()) return def;
  return std::stoi(s);
}

bool toBool(const std::string& s) {
  return s == "1" || s == "true" || s == "True";
}

}

void GameDatabase::LoadAll(const std::string& data_dir) {
  const std::string d = data_dir + "/";
  LoadGameConfig(d);
  LoadBodyPartConfig(d);
  LoadCharacters(d);
  LoadEnemies(d);
  LoadCastles(d);
  LoadRooms(d);
  LoadUsableItems(d);
  LoadDialogues(d);
  LoadDialogueChoices(d);
  LoadEvents(d);
  LoadShopItems(d);
  LoadMerchants(d);
}

void GameDatabase::LoadGameConfig(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "game_config.csv");
  for (auto& row : rows) {
    if (row.size() < 2) continue;
    game_config_[row[0]] = row[1];
  }
}

void GameDatabase::LoadBodyPartConfig(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "body_part_config.csv");
  for (auto& row : rows) {
    if (row.size() < 4) continue;
    BodyPartConfig cfg;
    cfg.id                   = toInt(row[0]);
    cfg.part_name            = row[1];
    cfg.hit_chance_base      = toInt(row[2]);
    cfg.damage_multiplier_pct= toInt(row[3]);
    body_part_configs_.push_back(cfg);
  }
}

void GameDatabase::LoadCharacters(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "characters.csv");
  for (auto& row : rows) {
    if (row.size() < 10) continue;
    CharacterData d;
    d.id            = toInt(row[0]);
    d.name          = row[1];
    d.attack        = toInt(row[2]);
    d.defense       = toInt(row[3]);
    d.agility       = toInt(row[4]);
    d.head_hp       = toInt(row[5]);
    d.body_hp       = toInt(row[6]);
    d.arms_hp       = toInt(row[7]);
    d.legs_hp       = toInt(row[8]);
    d.starting_gold = toInt(row[9]);
    characters_.push_back(d);
  }
}

void GameDatabase::LoadEnemies(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "enemies.csv");
  for (auto& row : rows) {
    if (row.size() < 11) continue;
    EnemyData d;
    d.id          = toInt(row[0]);
    d.name        = row[1];
    d.attack      = toInt(row[2]);
    d.defense     = toInt(row[3]);
    d.agility     = toInt(row[4]);
    d.head_hp     = toInt(row[5]);
    d.body_hp     = toInt(row[6]);
    d.arms_hp     = toInt(row[7]);
    d.legs_hp     = toInt(row[8]);
    d.reward_gold = toInt(row[9]);
    d.description = row[10];
    enemies_.push_back(d);
  }
}

void GameDatabase::LoadCastles(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "castles.csv");
  for (auto& row : rows) {
    if (row.size() < 3) continue;
    CastleData d;
    d.id         = toInt(row[0]);
    d.name       = row[1];
    d.intro_text = row[2];
    castles_.push_back(d);
  }
}

void GameDatabase::LoadRooms(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "rooms.csv");
  for (auto& row : rows) {
    if (row.size() < 10) continue;
    RoomData d;
    d.id            = toInt(row[0]);
    d.castle_id     = toInt(row[1]);
    d.room_number   = toInt(row[2]);
    d.enemy_id      = toInt(row[3]);
    d.item_id       = toInt(row[4]);
    d.item_type     = row[5];
    d.has_hint      = toBool(row[6]);
    d.hint_text     = row[7];
    d.search_chance = toInt(row[8]);
    d.description   = row[9];
    rooms_.push_back(d);
  }
}

void GameDatabase::LoadUsableItems(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "usable_items.csv");
  for (auto& row : rows) {
    if (row.size() < 7) continue;
    UsableItemData d;
    d.id             = toInt(row[0]);
    d.name           = row[1];
    d.type           = row[2];
    d.value          = toInt(row[3]);
    d.duration_turns = toInt(row[4]);
    d.description    = row[5];
    d.price          = toInt(row[6]);
    usable_items_.push_back(d);
  }
}

void GameDatabase::LoadDialogues(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "dialogues.csv");
  for (auto& row : rows) {
    if (row.size() < 4) continue;
    DialogueNode d;
    d.id       = toInt(row[0]);
    d.event_id = toInt(row[1]);
    d.speaker  = row[2];
    d.text     = row[3];
    dialogues_.push_back(d);
  }
}

void GameDatabase::LoadDialogueChoices(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "dialogue_choices.csv");
  for (auto& row : rows) {
    if (row.size() < 7) continue;
    DialogueChoiceData d;
    d.id                 = toInt(row[0]);
    d.dialogue_id        = toInt(row[1]);
    d.choice_number      = toInt(row[2]);
    d.choice_text        = row[3];
    d.next_dialogue_id   = toInt(row[4], -1);
    d.effect_type        = row[5];
    d.effect_value       = row[6];
    dialogue_choices_.push_back(d);
  }
}

void GameDatabase::LoadEvents(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "events.csv");
  for (auto& row : rows) {
    if (row.size() < 3) continue;
    EventData d;
    d.id                = toInt(row[0]);
    d.location          = row[1];
    d.start_dialogue_id = toInt(row[2]);
    events_.push_back(d);
  }
}

void GameDatabase::LoadShopItems(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "shop_items.csv");
  for (auto& row : rows) {
    if (row.size() < 5) continue;
    ShopItemData d;
    d.id             = toInt(row[0]);
    d.shop_id        = toInt(row[1]);
    d.item_type      = row[2];
    d.item_id        = toInt(row[3]);
    d.price_override = toInt(row[4]);
    shop_items_.push_back(d);
  }
}

void GameDatabase::LoadMerchants(const std::string& dir) {
  auto rows = CsvParser::Parse(dir + "merchants.csv");
  for (auto& row : rows) {
    if (row.size() < 4) continue;
    MerchantData d;
    d.id       = toInt(row[0]);
    d.name     = row[1];
    d.shop_id  = toInt(row[2]);
    d.location = row[3];
    merchants_.push_back(d);
  }
}
