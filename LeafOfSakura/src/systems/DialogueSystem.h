#pragma once
#include "../data/GameDatabase.h"
#include <string>
#include <vector>

struct DialogueEffect {
  std::string type;
  std::string value;
};

class DialogueSystem {
public:
  explicit DialogueSystem(const GameDatabase& db);

  std::vector<DialogueEffect> StartDialogue(int start_node_id);

private:
  void ShowNode(const DialogueNode& node) const;
  int  AskChoice(const std::vector<DialogueChoiceData>& choices) const;
  int  ReadInt(int min_val, int max_val) const;

  const GameDatabase& db_;
};
