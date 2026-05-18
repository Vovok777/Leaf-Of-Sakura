#include "Enemy.h"
#include <random>

namespace {
std::mt19937 s_enemy_rng{ std::random_device{}() };
}

Enemy::Enemy(const std::string& name, int atk, int def, int agi,
             const BodyParts& parts, int reward_gold, const std::string& description,
             int arms_penalty, int legs_penalty)
  : Character(name, atk, def, agi, parts, arms_penalty, legs_penalty)
  , reward_gold_(reward_gold)
  , description_(description)
{}

int Enemy::ChooseAttackPart() const {
  std::uniform_int_distribution<int> dist(0, 3);
  return dist(s_enemy_rng);
}

int Enemy::GetRewardGold() const {
  return reward_gold_;
}

const std::string& Enemy::GetDescription() const {
  return description_;
}
