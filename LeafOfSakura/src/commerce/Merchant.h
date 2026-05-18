#pragma once
#include "Shop.h"
#include <string>

class Hero;

class Merchant {
public:
  Merchant(const std::string& name, const std::string& location, Shop shop);

  const std::string& GetName()     const;
  const std::string& GetLocation() const;

  Shop&       GetShop();
  const Shop& GetShop() const;

private:
  std::string name_;
  std::string location_;
  Shop shop_;
};
