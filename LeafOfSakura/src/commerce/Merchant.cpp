#include "Merchant.h"

Merchant::Merchant(const std::string& name, const std::string& location, Shop shop)
  : name_(name)
  , location_(location)
  , shop_(std::move(shop))
{
}

const std::string& Merchant::GetName()     const { return name_; }
const std::string& Merchant::GetLocation() const { return location_; }

Shop&       Merchant::GetShop()       { return shop_; }
const Shop& Merchant::GetShop() const { return shop_; }
