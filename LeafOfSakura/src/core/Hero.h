#pragma once
#include "Character.h"

class Inventory;

class Hero : public Character {
public:
  Hero(const std::string& name, int atk, int def, int agi,
       const BodyParts& parts, int arms_penalty = 3, int legs_penalty = 3);

  void       SetInventory(Inventory* inv);
  Inventory* GetInventory() const;

  int  GetGold() const;
  void AddGold(int amount);
  bool SpendGold(int amount);

private:
  Inventory* inventory_ = nullptr;
  int gold_ = 0;
};
