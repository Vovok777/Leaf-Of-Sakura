#include "BattleSystem.h"
#include "ConsoleUtils.h"
#include <iostream>

namespace {
const std::string kSep  = "════════════════════════════════════════════════════════════════════════════════";
const std::string kLine = "────────────────────────────────────────────────────────────────────────────────";
}

void BattleSystem::DrawBattleScreen(const Hero& hero, const Enemy& enemy) const {
  ClearScreen();
  std::cout << kSep << "\n";
  std::cout << "  БОЙ: " << hero.GetName() << "  VS  " << enemy.GetName()
            << "  [ Ход: " << turn_ << " ]\n";
  std::cout << kSep << "\n\n";

  DrawCharacterSection(hero);
  std::cout << "\n";
  DrawCharacterSection(enemy);

  std::cout << "\n" << kLine << "\n";
  if (log_.empty()) {
    std::cout << "  ...\n";
  } else {
    for (const auto& entry : log_)
      std::cout << "  " << entry << "\n";
  }
  std::cout << kLine << "\n";
}

void BattleSystem::DrawCharacterSection(const Character& ch) const {
  const BodyParts& p  = ch.GetBodyParts();
  const BodyParts& mp = ch.GetMaxBodyParts();

  std::cout << "  " << ch.GetName()
            << "   АТК:" << ch.GetAttack()
            << "  ЗАЩ:" << ch.GetDefense()
            << "  ЛОВ:" << ch.GetAgility() << "\n";
  std::cout << "    Голова: (" << p.head << "/" << mp.head << ")"
            << "   Тело: (" << p.body << "/" << mp.body << ")\n";
  std::cout << "    Руки:   (" << p.arms << "/" << mp.arms << ")"
            << "   Ноги: (" << p.legs << "/" << mp.legs << ")\n";
}
