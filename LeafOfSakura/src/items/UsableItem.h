#pragma once
#include "Item.h"

class Hero;

struct UsableItemData {
  int id;
  std::string name;
  std::string type;
  int value;
  int duration_turns;
  std::string description;
  int price;
};

class UsableItem : public Item {
public:
  explicit UsableItem(const UsableItemData& data);

  int GetPrice()    const;
  int GetValue()    const;
  int GetDuration() const;
  std::string GetDescription() const override;

  void UseOnHero(Hero& hero) const;

private:
  int value_;
  int duration_turns_;
  int price_;
  std::string description_;
};
