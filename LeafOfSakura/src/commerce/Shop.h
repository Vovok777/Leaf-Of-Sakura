#pragma once
#include "../items/Item.h"
#include <vector>
#include <memory>

struct ShopEntry {
  std::shared_ptr<Item> item;
  int price;
};

class Shop {
public:
  void AddEntry(ShopEntry entry);

  const std::vector<ShopEntry>& GetEntries() const;

private:
  std::vector<ShopEntry> entries_;
};
