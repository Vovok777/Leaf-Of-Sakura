#pragma once
#include "Character.h"

class Enemy : public Character {
public:
  Enemy(const std::string& name, int atk, int def, int agi,
        const BodyParts& parts, int reward_gold, const std::string& description,
        int arms_penalty = 3, int legs_penalty = 3);

  int ChooseAttackPart() const;

  int                GetRewardGold()   const;
  const std::string& GetDescription() const;

private:
  int reward_gold_;
  std::string description_;
};
