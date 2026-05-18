#pragma once
#include "Room.h"
#include <string>
#include <vector>

struct CastleData {
  int id;
  std::string name;
  std::string intro_text;
};

class Castle {
public:
  Castle(const CastleData& data, std::vector<Room> rooms);

  std::vector<Room>&       GetRooms();
  const std::vector<Room>& GetRooms() const;

  void MarkCleared();

private:
  CastleData data_;
  std::vector<Room> rooms_;
  bool cleared_ = false;
};
