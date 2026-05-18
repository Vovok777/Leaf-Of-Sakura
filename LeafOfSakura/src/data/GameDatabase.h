#pragma once
#include "../items/UsableItem.h"
#include "../world/Castle.h"
#include "../world/Room.h"
#include <map>
#include <string>
#include <vector>

struct CharacterData {
  int id;
  std::string name;
  int attack, defense, agility;
  int head_hp, body_hp, arms_hp, legs_hp;
  int starting_gold;
};

struct EnemyData {
  int id;
  std::string name;
  int attack, defense, agility;
  int head_hp, body_hp, arms_hp, legs_hp;
  int reward_gold;
  std::string description;
};

struct BodyPartConfig {
  int id;
  std::string part_name;
  int hit_chance_base;
  int damage_multiplier_pct;
};

struct DialogueNode {
  int id;
  int event_id;
  std::string speaker;
  std::string text;
};

struct DialogueChoiceData {
  int id;
  int dialogue_id;
  int choice_number;
  std::string choice_text;
  int next_dialogue_id;
  std::string effect_type;
  std::string effect_value;
};

struct EventData {
  int id;
  std::string location;
  int start_dialogue_id;
};

struct ShopItemData {
  int id;
  int shop_id;
  std::string item_type;
  int item_id;
  int price_override;
};

struct MerchantData {
  int id;
  std::string name;
  int shop_id;
  std::string location;
};

class GameDatabase {
public:
  void LoadAll(const std::string& data_dir);

  std::string GetConfig(const std::string& key,
                        const std::string& default_val = "") const;
  int         GetConfigInt(const std::string& key, int default_val = 0) const;

  const BodyPartConfig* GetBodyPartConfig(int part_id) const;

  const CharacterData*  GetCharacterById(int id) const;

  const EnemyData*      GetEnemyById(int id) const;

  const CastleData*          GetCastleById(int id) const;
  std::vector<RoomData>      GetRoomsForCastle(int castle_id) const;

  const UsableItemData*       GetUsableItemById(int id) const;

  const DialogueNode*              GetDialogueNodeById(int id) const;
  std::vector<DialogueChoiceData>  GetChoicesForDialogue(int dialogue_id) const;

  const EventData* GetEventByLocation(const std::string& location) const;

  std::vector<ShopItemData>  GetShopItems(int shop_id) const;
  const MerchantData*        GetMerchantByLocation(const std::string& location) const;

private:
  void LoadGameConfig(const std::string& dir);
  void LoadBodyPartConfig(const std::string& dir);
  void LoadCharacters(const std::string& dir);
  void LoadEnemies(const std::string& dir);
  void LoadCastles(const std::string& dir);
  void LoadRooms(const std::string& dir);
  void LoadUsableItems(const std::string& dir);
  void LoadDialogues(const std::string& dir);
  void LoadDialogueChoices(const std::string& dir);
  void LoadEvents(const std::string& dir);
  void LoadShopItems(const std::string& dir);
  void LoadMerchants(const std::string& dir);

  std::map<std::string, std::string>  game_config_;
  std::vector<BodyPartConfig>          body_part_configs_;
  std::vector<CharacterData>           characters_;
  std::vector<EnemyData>               enemies_;
  std::vector<CastleData>              castles_;
  std::vector<RoomData>                rooms_;
  std::vector<UsableItemData>          usable_items_;
  std::vector<DialogueNode>            dialogues_;
  std::vector<DialogueChoiceData>      dialogue_choices_;
  std::vector<EventData>               events_;
  std::vector<ShopItemData>            shop_items_;
  std::vector<MerchantData>            merchants_;
};
