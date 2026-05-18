#pragma once
#include <string>

struct RoomData {
  int id;
  int castle_id;
  int room_number;
  int enemy_id;
  int item_id;
  std::string item_type;
  bool has_hint;
  std::string hint_text;
  int search_chance;
  std::string description;
};

class Room {
public:
  explicit Room(const RoomData& data);

  const RoomData& GetData() const;

  bool IsEnemyDefeated()  const;
  bool IsItemCollected()  const;
  bool IsSearched()       const;

  void MarkEnemyDefeated();
  void MarkItemCollected();
  void MarkSearched();

private:
  RoomData data_;
  bool enemy_defeated_ = false;
  bool item_collected_ = false;
  bool searched_       = false;
};
