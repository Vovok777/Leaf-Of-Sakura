#include "Hero.h"

Hero::Hero(const std::string& name, int atk, int def, int agi,
           const BodyParts& parts, int arms_penalty, int legs_penalty)
  : Character(name, atk, def, agi, parts, arms_penalty, legs_penalty)
  , inventory_(nullptr)
  , gold_(0)
{}

void Hero::SetInventory(Inventory* inv) {
  inventory_ = inv;
}

Inventory* Hero::GetInventory() const {
  return inventory_;
}

int Hero::GetGold() const {
  return gold_;
}

void Hero::AddGold(int amount) {
  if (amount > 0) gold_ += amount;
}

bool Hero::SpendGold(int amount) {
  if (gold_ < amount) return false;
  gold_ -= amount;
  return true;
}
