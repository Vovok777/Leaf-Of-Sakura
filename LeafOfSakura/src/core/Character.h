#pragma once
#include <string>
#include <vector>

struct BodyParts {
  int head;
  int body;
  int arms;
  int legs;
};

struct ActiveEffect {
  std::string stat;
  int value;
  int turns_left;
};

struct AttackParams {
  int hit_chance;
  int damage_multiplier_pct;
  int min_damage;
  int defense_divisor;
  int min_hit_chance;
};

class Character {
public:
  Character(const std::string& name, int atk, int def, int agi,
            const BodyParts& parts, int arms_penalty = 3, int legs_penalty = 3);
  virtual ~Character() = default;

  bool TakeDamageToHead(int incoming_attack, const AttackParams& p);
  bool TakeDamageToBody(int incoming_attack, const AttackParams& p);
  bool TakeDamageToArms(int incoming_attack, const AttackParams& p);
  bool TakeDamageToLegs(int incoming_attack, const AttackParams& p);

  int DirectDamage(int body_part, int amount);

  void HealHead(int value);
  void HealBody(int value);
  void HealArms(int value);
  void HealLegs(int value);
  void HealAll(int value);

  bool RollHit(int base_chance, int min_chance) const;

  void UpdateAliveState();

  void AddEffect(const ActiveEffect& effect);
  void UpdateEffects();

  int GetAttack()  const;
  int GetDefense() const;
  int GetAgility() const;

  bool IsAlive()       const;
  bool AreArmsBroken() const;
  bool AreLegsBroken() const;

  const std::string& GetName()         const;
  const BodyParts&   GetBodyParts()    const;
  const BodyParts&   GetMaxBodyParts() const;

protected:
  std::string name_;
  int attack_;
  int defense_;
  int agility_;
  BodyParts parts_;
  BodyParts max_parts_;
  bool alive_;
  bool arms_broken_;
  bool legs_broken_;
  std::vector<ActiveEffect> active_effects_;

private:
  int arms_broken_penalty_;
  int legs_broken_penalty_;
};
