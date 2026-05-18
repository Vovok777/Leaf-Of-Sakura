#include "BattleSystem.h"

#include <iostream>
#include <string>
#include <vector>

#include "../items/Inventory.h"
#include "../items/UsableItem.h"
#include "ConsoleUtils.h"

namespace {
const std::string kLine =
    "──────────────────────────────────────────────────────────────────────────"
    "──────";
const std::string kPartAcc[] = {"голову", "тело", "руки", "ноги"};

std::string FormatMult(int pct) {
  if (pct % 100 == 0) return "x" + std::to_string(pct / 100);
  return "x" + std::to_string(pct / 100) + "." +
         std::to_string((pct % 100) / 10);
}
}  // namespace

BattleSystem::BattleSystem(const GameDatabase& db) : db_(db) {}

bool BattleSystem::StartBattle(Hero& hero, Enemy& enemy) {
  turn_ = 0;
  log_.clear();
  if (!enemy.GetDescription().empty()) AddLog(enemy.GetDescription());

  while (hero.IsAlive() && enemy.IsAlive()) {
    ++turn_;

    PlayerTurn(hero, enemy);
    if (!enemy.IsAlive()) break;

    EnemyTurn(hero, enemy);

    hero.UpdateEffects();
    enemy.UpdateEffects();

    if (!hero.IsAlive()) break;

    DrawBattleScreen(hero, enemy);
    WaitEnter();
  }

  DrawBattleScreen(hero, enemy);
  std::cout << "\n" << kLine << "\n";

  if (hero.IsAlive()) {
    std::cout << "  Победа! " << enemy.GetName() << " повержен.\n";
    int gold = enemy.GetRewardGold();
    if (gold > 0) {
      hero.AddGold(gold);
      std::cout << "  Получено золото: +" << gold
                << "  (итого: " << hero.GetGold() << ")\n";
    }
    std::cout << kLine << "\n";
    WaitEnter();
    return true;
  }

  std::cout << "  " << hero.GetName() << " пал в бою...\n";
  std::cout << kLine << "\n";
  WaitEnter();
  return false;
}

void BattleSystem::PlayerTurn(Hero& hero, Enemy& enemy) {
  while (true) {
    DrawBattleScreen(hero, enemy);

    Inventory* inv = hero.GetInventory();

    bool has_usable = inv && !inv->IsEmpty();

    std::cout << "\n" << kLine << "\n";
    std::cout << "  Твой ход:\n";
    std::cout << "  1. Атаковать\n";
    if (has_usable) std::cout << "  2. Использовать предмет\n";

    int action = ReadInt(1, has_usable ? 2 : 1);

    if (action == 1) {
      const BodyPartConfig* bpc[4] = {
          db_.GetBodyPartConfig(0),
          db_.GetBodyPartConfig(1),
          db_.GetBodyPartConfig(2),
          db_.GetBodyPartConfig(3),
      };
      std::cout << "  Выбери зону атаки:\n";
      std::cout << "  1. Голова  ("
                << FormatMult(bpc[0] ? bpc[0]->damage_multiplier_pct : 300)
                << " урон, " << (bpc[0] ? bpc[0]->hit_chance_base : 20)
                << "%) — смертельная зона\n";
      std::cout << "  2. Тело    ("
                << FormatMult(bpc[1] ? bpc[1]->damage_multiplier_pct : 150)
                << " урон, " << (bpc[1] ? bpc[1]->hit_chance_base : 80)
                << "%)\n";
      std::cout << "  3. Руки    ("
                << FormatMult(bpc[2] ? bpc[2]->damage_multiplier_pct : 100)
                << " урон, " << (bpc[2] ? bpc[2]->hit_chance_base : 60)
                << "%)  — сломанные руки снижают атаку\n";
      std::cout << "  4. Ноги    ("
                << FormatMult(bpc[3] ? bpc[3]->damage_multiplier_pct : 80)
                << " урон, " << (bpc[3] ? bpc[3]->hit_chance_base : 65)
                << "%) — сломанные ноги снижают ловкость\n";
      HeroAttack(hero, enemy, ReadInt(1, 4) - 1);
      return;
    }

    ShowAndUseInventory(hero, enemy);
  }
}

AttackParams BattleSystem::MakeAttackParams(int part_id) const {
  AttackParams p;
  p.min_damage = db_.GetConfigInt("min_damage_per_hit", 3);
  p.defense_divisor = db_.GetConfigInt("defense_divisor", 3);
  p.min_hit_chance = db_.GetConfigInt("min_hit_chance", 10);
  const BodyPartConfig* bpc = db_.GetBodyPartConfig(part_id);
  p.hit_chance = bpc ? bpc->hit_chance_base : 50;
  p.damage_multiplier_pct = bpc ? bpc->damage_multiplier_pct : 100;
  return p;
}

void BattleSystem::HeroAttack(Hero& hero, Enemy& enemy, int body_part) {
  if (body_part < 0 || body_part > 3) body_part = 1;

  const BodyParts& bp0 = enemy.GetBodyParts();
  int hp0 = 0;
  switch (body_part) {
    case 0:
      hp0 = bp0.head;
      break;
    case 1:
      hp0 = bp0.body;
      break;
    case 2:
      hp0 = bp0.arms;
      break;
    case 3:
      hp0 = bp0.legs;
      break;
  }

  int atk = hero.GetAttack();
  bool hit = false;
  AttackParams p = MakeAttackParams(body_part);
  switch (body_part) {
    case 0:
      hit = enemy.TakeDamageToHead(atk, p);
      break;
    case 1:
      hit = enemy.TakeDamageToBody(atk, p);
      break;
    case 2:
      hit = enemy.TakeDamageToArms(atk, p);
      break;
    case 3:
      hit = enemy.TakeDamageToLegs(atk, p);
      break;
  }

  if (hit) {
    const BodyParts& bp1 = enemy.GetBodyParts();
    int hp1 = 0;
    switch (body_part) {
      case 0:
        hp1 = bp1.head;
        break;
      case 1:
        hp1 = bp1.body;
        break;
      case 2:
        hp1 = bp1.arms;
        break;
      case 3:
        hp1 = bp1.legs;
        break;
    }
    int dmg = hp0 - hp1;
    std::string msg = "► " + hero.GetName() + " → " + kPartAcc[body_part] +
                      ": ПОПАДАНИЕ! Урон: " + std::to_string(dmg);
    if (body_part == 2 && enemy.AreArmsBroken()) msg += "  [руки сломаны!]";
    if (body_part == 3 && enemy.AreLegsBroken()) msg += "  [ноги сломаны!]";
    AddLog(msg);
  } else {
    AddLog("► " + hero.GetName() + " → " + kPartAcc[body_part] + ": промах!");
  }
}

void BattleSystem::EnemyTurn(Hero& hero, Enemy& enemy) {
  int part = enemy.ChooseAttackPart();

  const BodyParts& bp0 = hero.GetBodyParts();
  int hp0 = 0;
  switch (part) {
    case 0:
      hp0 = bp0.head;
      break;
    case 1:
      hp0 = bp0.body;
      break;
    case 2:
      hp0 = bp0.arms;
      break;
    case 3:
      hp0 = bp0.legs;
      break;
  }

  int atk = enemy.GetAttack();
  bool hit = false;
  AttackParams p = MakeAttackParams(part);
  switch (part) {
    case 0:
      hit = hero.TakeDamageToHead(atk, p);
      break;
    case 1:
      hit = hero.TakeDamageToBody(atk, p);
      break;
    case 2:
      hit = hero.TakeDamageToArms(atk, p);
      break;
    case 3:
      hit = hero.TakeDamageToLegs(atk, p);
      break;
  }

  if (hit) {
    const BodyParts& bp1 = hero.GetBodyParts();
    int hp1 = 0;
    switch (part) {
      case 0:
        hp1 = bp1.head;
        break;
      case 1:
        hp1 = bp1.body;
        break;
      case 2:
        hp1 = bp1.arms;
        break;
      case 3:
        hp1 = bp1.legs;
        break;
    }
    int dmg = hp0 - hp1;
    std::string msg = "► " + enemy.GetName() + " → " + kPartAcc[part] +
                      ": ПОПАДАНИЕ! Урон: " + std::to_string(dmg);
    if (part == 2 && hero.AreArmsBroken()) msg += "  [твои руки повреждены!]";
    if (part == 3 && hero.AreLegsBroken()) msg += "  [твои ноги повреждены!]";
    AddLog(msg);
  } else {
    AddLog("► " + enemy.GetName() + " → " + kPartAcc[part] + ": промах!");
  }
}

bool BattleSystem::ShowAndUseInventory(Hero& hero, Enemy& enemy) {
  Inventory* inv = hero.GetInventory();
  if (!inv || inv->IsEmpty()) return false;

  const auto& items = inv->GetItems();

  std::cout << "\n  Предметы:\n";
  for (int i = 0; i < static_cast<int>(items.size()); ++i) {
    std::cout << "  " << (i + 1) << ". " << items[i]->GetName() << "\n";
    std::string desc = items[i]->GetDescription();
    if (!desc.empty()) std::cout << "     " << desc << "\n";
  }
  std::cout << "  0. Назад\n";

  int choice = ReadInt(0, static_cast<int>(items.size()));
  if (choice == 0) return false;

  int inv_i = choice - 1;
  auto usable = std::static_pointer_cast<UsableItem>(items[inv_i]);

  const std::string item_name = usable->GetName();
  const std::string item_type = usable->GetType();

  if (item_type == "throwable") {
    std::cout << "  Выбери зону врага:\n";
    std::cout << "  1. Голова  2. Тело  3. Руки  4. Ноги\n";
    int part = ReadInt(1, 4) - 1;

    int accuracy = usable->GetDuration();
    AttackParams tp = MakeAttackParams(part);
    bool hit = enemy.RollHit(accuracy, tp.min_hit_chance);

    inv->RemoveItem(inv_i);

    if (hit) {
      int dmg = enemy.DirectDamage(part, usable->GetValue());
      AddLog("► " + item_name + " → " + kPartAcc[part] +
             ": попадание! Урон: " + std::to_string(dmg));
    } else {
      AddLog("► " + item_name + " → " + kPartAcc[part] +
             ": промах! (предмет потрачен)");
    }
    return true;
  }

  if (item_type.find("medicine") != std::string::npos) {
    int val_full = usable->GetValue();
    int val_all = val_full / 3;
    if (val_all < 1) val_all = 1;

    std::cout << "  Как применить " << item_name << "?\n";
    std::cout << "  1. Одну часть тела  (+" << val_full << " HP)\n";
    std::cout << "  2. Все части тела   (+" << val_all << " HP каждой)\n";
    std::cout << "  0. Отмена\n";
    int med = ReadInt(0, 2);

    if (med == 0) return false;

    if (med == 1) {
      std::cout
          << "  Что лечить?  1=голова  2=тело  3=руки  4=ноги  0=отмена\n";
      int part = ReadInt(0, 4);
      if (part == 0) return false;

      const BodyParts& cur = hero.GetBodyParts();
      const BodyParts& mx = hero.GetMaxBodyParts();
      int c = 0, m = 0;
      switch (part) {
        case 1:
          c = cur.head;
          m = mx.head;
          break;
        case 2:
          c = cur.body;
          m = mx.body;
          break;
        case 3:
          c = cur.arms;
          m = mx.arms;
          break;
        case 4:
          c = cur.legs;
          m = mx.legs;
          break;
      }
      if (c >= m) {
        AddLog("► " + kPartAcc[part - 1] +
               " уже в норме — предмет не потрачен.");
        return false;
      }

      inv->RemoveItem(inv_i);
      switch (part) {
        case 1:
          hero.HealHead(val_full);
          break;
        case 2:
          hero.HealBody(val_full);
          break;
        case 3:
          hero.HealArms(val_full);
          break;
        case 4:
          hero.HealLegs(val_full);
          break;
      }
      AddLog("► " + hero.GetName() + " исп. " + item_name + " → " +
             kPartAcc[part - 1] + " +" + std::to_string(val_full) + " HP.");
      return true;
    }

    inv->RemoveItem(inv_i);
    hero.HealAll(val_all);
    AddLog("► " + hero.GetName() + " исп. " + item_name + " → все части +" +
           std::to_string(val_all) + " HP.");
    return true;
  }

  inv->UseItem(inv_i, hero);
  AddLog("► " + hero.GetName() + " использовал: " + item_name + ".");
  return true;
}

void BattleSystem::AddLog(const std::string& entry) {
  log_.push_back(entry);
  int max_log = db_.GetConfigInt("battle_log_size", 5);
  while (static_cast<int>(log_.size()) > max_log) log_.pop_front();
}

void BattleSystem::WaitEnter() const {
  std::cout << "  [Нажми Enter...]\n  > ";
  std::cin.ignore(10000, '\n');
}

int BattleSystem::ReadInt(int min_val, int max_val) const {
  int value;
  while (true) {
    std::cout << "  > ";
    if (std::cin >> value && value >= min_val && value <= max_val) break;
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cout << "  Введи число от " << min_val << " до " << max_val << ".\n";
  }
  std::cin.ignore(10000, '\n');
  return value;
}
