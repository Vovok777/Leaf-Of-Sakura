#pragma once
#include "Item.h"
#include <vector>
#include <memory>

class Hero;

class Inventory {
public:
  explicit Inventory(int max_size);

  bool AddItem(std::shared_ptr<Item> item);
  void RemoveItem(int index);

  void UseItem(int index, Hero& hero);

  const std::vector<std::shared_ptr<Item>>& GetItems() const;
  bool IsFull()  const;
  bool IsEmpty() const;
  int  Size()    const;

private:
  std::vector<std::shared_ptr<Item>> items_;
  int max_size_;
};
