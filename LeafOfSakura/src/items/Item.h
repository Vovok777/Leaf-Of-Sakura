#pragma once
#include <string>

class Item {
public:
  Item(int id, const std::string& name, const std::string& type)
    : id_(id), name_(name), type_(type) {}

  virtual ~Item() = default;

  int                GetId()   const { return id_; }
  const std::string& GetName() const { return name_; }
  const std::string& GetType() const { return type_; }

  virtual std::string GetDescription() const { return ""; }

protected:
  int id_;
  std::string name_;
  std::string type_;
};
