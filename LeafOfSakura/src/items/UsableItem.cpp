#include "UsableItem.h"
#include "../core/Hero.h"

UsableItem::UsableItem(const UsableItemData& data)
  : Item(data.id, data.name, data.type)
  , value_(data.value)
  , duration_turns_(data.duration_turns)
  , price_(data.price)
  , description_(data.description)
{
}

int         UsableItem::GetPrice()       const { return price_; }
int         UsableItem::GetValue()       const { return value_; }
int         UsableItem::GetDuration()    const { return duration_turns_; }
std::string UsableItem::GetDescription() const {
  std::string result = description_;

  auto replace_all = [&](const std::string& placeholder, const std::string& replacement) {
    std::size_t pos = 0;
    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
      result.replace(pos, placeholder.size(), replacement);
      pos += replacement.size();
    }
  };

  replace_all("{value}",    std::to_string(value_));
  replace_all("{duration}", std::to_string(duration_turns_));

  return result;
}

void UsableItem::UseOnHero(Hero& hero) const {
  if (type_ == "medicine_all")  { hero.HealAll(value_);  return; }
  if (type_ == "medicine_head") { hero.HealHead(value_); return; }
  if (type_ == "medicine_body") { hero.HealBody(value_); return; }
  if (type_ == "medicine_arms") { hero.HealArms(value_); return; }
  if (type_ == "medicine_legs") { hero.HealLegs(value_); return; }

  ActiveEffect effect;
  effect.value      = value_;
  effect.turns_left = duration_turns_;

  if (type_ == "smoke_bomb")    effect.stat = "agility";
  if (type_ == "attack_boost")  effect.stat = "attack";
  if (type_ == "defense_boost") effect.stat = "defense";

  if (!effect.stat.empty())
    hero.AddEffect(effect);
}
