#pragma once
#include "../core/Hero.h"
#include "../core/Enemy.h"
#include "../data/GameDatabase.h"
#include <deque>
#include <string>

class BattleSystem {
public:
  explicit BattleSystem(const GameDatabase& db);

  bool StartBattle(Hero& hero, Enemy& enemy);

private:
  void PlayerTurn(Hero& hero, Enemy& enemy);
  void EnemyTurn(Hero& hero, Enemy& enemy);
  void HeroAttack(Hero& hero, Enemy& enemy, int body_part);
  bool ShowAndUseInventory(Hero& hero, Enemy& enemy);
  void DrawBattleScreen(const Hero& hero, const Enemy& enemy) const;
  void DrawCharacterSection(const Character& ch) const;
  void AddLog(const std::string& entry);
  AttackParams MakeAttackParams(int part_id) const;

  void WaitEnter() const;
  int  ReadInt(int min_val, int max_val) const;

  const GameDatabase& db_;
  std::deque<std::string> log_;
  int turn_ = 0;
};
