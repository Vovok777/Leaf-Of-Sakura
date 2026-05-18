#include "GameDatabase.h"
#include <algorithm>

namespace {

template<typename T>
const T* FindById(const std::vector<T>& vec, int id) {
  for (const auto& item : vec)
    if (item.id == id) return &item;
  return nullptr;
}

}

std::string GameDatabase::GetConfig(const std::string& key,
                                    const std::string& default_val) const {
  auto it = game_config_.find(key);
  return it != game_config_.end() ? it->second : default_val;
}

int GameDatabase::GetConfigInt(const std::string& key, int default_val) const {
  auto it = game_config_.find(key);
  if (it == game_config_.end()) return default_val;
  return std::stoi(it->second);
}

const BodyPartConfig* GameDatabase::GetBodyPartConfig(int id) const {
  return FindById(body_part_configs_, id);
}

const CharacterData* GameDatabase::GetCharacterById(int id) const {
  return FindById(characters_, id);
}

const EnemyData* GameDatabase::GetEnemyById(int id) const {
  return FindById(enemies_, id);
}

const CastleData* GameDatabase::GetCastleById(int id) const {
  return FindById(castles_, id);
}

const UsableItemData* GameDatabase::GetUsableItemById(int id) const {
  return FindById(usable_items_, id);
}

const DialogueNode* GameDatabase::GetDialogueNodeById(int id) const {
  return FindById(dialogues_, id);
}

std::vector<DialogueChoiceData> GameDatabase::GetChoicesForDialogue(int dialogue_id) const {
  std::vector<DialogueChoiceData> result;
  for (const auto& c : dialogue_choices_)
    if (c.dialogue_id == dialogue_id)
      result.push_back(c);
  std::sort(result.begin(), result.end(),
            [](const DialogueChoiceData& a, const DialogueChoiceData& b) {
              return a.choice_number < b.choice_number;
            });
  return result;
}

const EventData* GameDatabase::GetEventByLocation(const std::string& location) const {
  for (const auto& e : events_)
    if (e.location == location) return &e;
  return nullptr;
}

std::vector<RoomData> GameDatabase::GetRoomsForCastle(int castle_id) const {
  std::vector<RoomData> result;
  for (const auto& r : rooms_)
    if (r.castle_id == castle_id)
      result.push_back(r);
  std::sort(result.begin(), result.end(),
            [](const RoomData& a, const RoomData& b) {
              return a.room_number < b.room_number;
            });
  return result;
}

std::vector<ShopItemData> GameDatabase::GetShopItems(int shop_id) const {
  std::vector<ShopItemData> result;
  for (const auto& s : shop_items_)
    if (s.shop_id == shop_id)
      result.push_back(s);
  return result;
}

const MerchantData* GameDatabase::GetMerchantByLocation(const std::string& location) const {
  for (const auto& m : merchants_)
    if (m.location == location) return &m;
  return nullptr;
}
