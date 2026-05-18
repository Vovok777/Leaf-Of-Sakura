#include "Castle.h"

Castle::Castle(const CastleData& data, std::vector<Room> rooms)
  : data_(data)
  , rooms_(std::move(rooms))
{
}

std::vector<Room>&       Castle::GetRooms()       { return rooms_; }
const std::vector<Room>& Castle::GetRooms() const { return rooms_; }

void Castle::MarkCleared() { cleared_ = true; }
