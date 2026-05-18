#include "Character.h"
#include <algorithm>
#include <random>

namespace {
std::mt19937 s_rng{ std::random_device{}() };
}

Character::Character(const std::string& name, int atk, int def, int agi,
                     const BodyParts& parts, int arms_penalty, int legs_penalty)
  : name_(name)
  , attack_(atk)
  , defense_(def)
  , agility_(agi)
  , parts_(parts)
  , max_parts_(parts)
  , alive_(true)
  , arms_broken_(false)
  , legs_broken_(false)
  , arms_broken_penalty_(arms_penalty)
  , legs_broken_penalty_(legs_penalty)
{}

bool Character::TakeDamageToHead(int incoming_attack, const AttackParams& p) {
  if (!RollHit(p.hit_chance, p.min_hit_chance)) return false;
  int damage = incoming_attack * p.damage_multiplier_pct / 100 - GetDefense() / p.defense_divisor;
  if (damage < p.min_damage) damage = p.min_damage;
  parts_.head -= damage;
  if (parts_.head < 0) parts_.head = 0;
  UpdateAliveState();
  return true;
}

bool Character::TakeDamageToBody(int incoming_attack, const AttackParams& p) {
  if (!RollHit(p.hit_chance, p.min_hit_chance)) return false;
  int damage = incoming_attack * p.damage_multiplier_pct / 100 - GetDefense() / p.defense_divisor;
  if (damage < p.min_damage) damage = p.min_damage;
  parts_.body -= damage;
  if (parts_.body < 0) parts_.body = 0;
  UpdateAliveState();
  return true;
}

bool Character::TakeDamageToArms(int incoming_attack, const AttackParams& p) {
  if (!RollHit(p.hit_chance, p.min_hit_chance)) return false;
  int damage = incoming_attack * p.damage_multiplier_pct / 100 - GetDefense() / p.defense_divisor;
  if (damage < p.min_damage) damage = p.min_damage;
  parts_.arms -= damage;
  if (parts_.arms <= 0) {
    parts_.arms = 0;
    arms_broken_ = true;
  }
  UpdateAliveState();
  return true;
}

bool Character::TakeDamageToLegs(int incoming_attack, const AttackParams& p) {
  if (!RollHit(p.hit_chance, p.min_hit_chance)) return false;
  int damage = incoming_attack * p.damage_multiplier_pct / 100 - GetDefense() / p.defense_divisor;
  if (damage < p.min_damage) damage = p.min_damage;
  parts_.legs -= damage;
  if (parts_.legs <= 0) {
    parts_.legs = 0;
    legs_broken_ = true;
  }
  UpdateAliveState();
  return true;
}

int Character::DirectDamage(int body_part, int amount) {
  switch (body_part) {
    case 0: parts_.head -= amount; if (parts_.head < 0) parts_.head = 0; break;
    case 1: parts_.body -= amount; if (parts_.body < 0) parts_.body = 0; break;
    case 2: parts_.arms -= amount;
            if (parts_.arms <= 0) { parts_.arms = 0; arms_broken_ = true; }
            break;
    case 3: parts_.legs -= amount;
            if (parts_.legs <= 0) { parts_.legs = 0; legs_broken_ = true; }
            break;
    default: return 0;
  }
  UpdateAliveState();
  return amount;
}

void Character::HealHead(int value) {
  parts_.head = std::min(parts_.head + value, max_parts_.head);
  UpdateAliveState();
}

void Character::HealBody(int value) {
  parts_.body = std::min(parts_.body + value, max_parts_.body);
  UpdateAliveState();
}

void Character::HealArms(int value) {
  parts_.arms = std::min(parts_.arms + value, max_parts_.arms);
  if (parts_.arms > 0) arms_broken_ = false;
}

void Character::HealLegs(int value) {
  parts_.legs = std::min(parts_.legs + value, max_parts_.legs);
  if (parts_.legs > 0) legs_broken_ = false;
}

void Character::HealAll(int value) {
  HealHead(value);
  HealBody(value);
  HealArms(value);
  HealLegs(value);
}

bool Character::RollHit(int base_chance, int min_chance) const {
  int final_chance = base_chance - GetAgility();
  if (final_chance < min_chance) final_chance = min_chance;
  std::uniform_int_distribution<int> dist(1, 100);
  return dist(s_rng) <= final_chance;
}

void Character::UpdateAliveState() {
  if (parts_.head <= 0 || parts_.body <= 0)
    alive_ = false;
}

void Character::AddEffect(const ActiveEffect& effect) {
  active_effects_.push_back(effect);
}

void Character::UpdateEffects() {
  for (int i = static_cast<int>(active_effects_.size()) - 1; i >= 0; --i) {
    active_effects_[i].turns_left--;
    if (active_effects_[i].turns_left <= 0)
      active_effects_.erase(active_effects_.begin() + i);
  }
}

int Character::GetAttack() const {
  int total = attack_;
  for (const auto& e : active_effects_)
    if (e.stat == "attack") total += e.value;
  if (arms_broken_) total -= arms_broken_penalty_;
  if (total < 1) total = 1;
  return total;
}

int Character::GetDefense() const {
  int total = defense_;
  for (const auto& e : active_effects_)
    if (e.stat == "defense") total += e.value;
  return std::max(total, 0);
}

int Character::GetAgility() const {
  int total = agility_;
  for (const auto& e : active_effects_)
    if (e.stat == "agility") total += e.value;
  if (legs_broken_) total -= legs_broken_penalty_;
  if (total < 0) total = 0;
  return total;
}

bool Character::IsAlive()       const { return alive_; }
bool Character::AreArmsBroken() const { return arms_broken_; }
bool Character::AreLegsBroken() const { return legs_broken_; }

const std::string& Character::GetName()         const { return name_; }
const BodyParts&   Character::GetBodyParts()    const { return parts_; }
const BodyParts&   Character::GetMaxBodyParts() const { return max_parts_; }
