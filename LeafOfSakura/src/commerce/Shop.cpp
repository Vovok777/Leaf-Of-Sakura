#include "Shop.h"

void Shop::AddEntry(ShopEntry entry) {
  entries_.push_back(std::move(entry));
}

const std::vector<ShopEntry>& Shop::GetEntries() const {
  return entries_;
}
