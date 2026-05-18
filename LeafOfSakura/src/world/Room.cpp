#include "Room.h"

Room::Room(const RoomData& data) : data_(data) {}

const RoomData& Room::GetData() const { return data_; }

bool Room::IsEnemyDefeated() const { return enemy_defeated_; }
bool Room::IsItemCollected() const { return item_collected_; }
bool Room::IsSearched()      const { return searched_; }

void Room::MarkEnemyDefeated() { enemy_defeated_ = true; }
void Room::MarkItemCollected() { item_collected_ = true; }
void Room::MarkSearched()      { searched_ = true; }
