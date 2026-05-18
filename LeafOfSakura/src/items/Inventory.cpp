#include "Inventory.h"
#include "UsableItem.h"
#include "../core/Hero.h"

Inventory::Inventory(int max_size) : max_size_(max_size) {}

bool Inventory::AddItem(std::shared_ptr<Item> item) {
  if (IsFull()) return false;
  items_.push_back(std::move(item));
  return true;
}

void Inventory::RemoveItem(int index) {
  if (index < 0 || index >= static_cast<int>(items_.size())) return;
  items_.erase(items_.begin() + index);
}

void Inventory::UseItem(int index, Hero& hero) {
  if (index < 0 || index >= static_cast<int>(items_.size())) return;

  if (auto usable = std::dynamic_pointer_cast<UsableItem>(items_[index])) {
    usable->UseOnHero(hero);
    RemoveItem(index);
  }
}

const std::vector<std::shared_ptr<Item>>& Inventory::GetItems() const {
  return items_;
}

bool Inventory::IsFull()  const { return static_cast<int>(items_.size()) >= max_size_; }
bool Inventory::IsEmpty() const { return items_.empty(); }
int  Inventory::Size()    const { return static_cast<int>(items_.size()); }
