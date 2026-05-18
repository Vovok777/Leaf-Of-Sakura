#include "DialogueSystem.h"
#include "ConsoleUtils.h"
#include <iostream>
#include <string>
#include <vector>

namespace {
const std::string kDlgLine =
    "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
}

DialogueSystem::DialogueSystem(const GameDatabase& db) : db_(db) {}

std::vector<DialogueEffect> DialogueSystem::StartDialogue(int start_node_id) {
  std::vector<DialogueEffect> effects;
  int current_id = start_node_id;

  while (current_id != -1) {
    const DialogueNode* node = db_.GetDialogueNodeById(current_id);
    if (!node) break;

    ShowNode(*node);

    auto choices = db_.GetChoicesForDialogue(current_id);
    if (choices.empty()) break;

    int chosen_idx = AskChoice(choices);
    const DialogueChoiceData& choice = choices[chosen_idx];

    if (choice.effect_type != "none" && !choice.effect_type.empty()) {
      effects.push_back({ choice.effect_type, choice.effect_value });
    }

    if (choice.effect_type == "start_battle") break;

    current_id = choice.next_dialogue_id;
  }

  return effects;
}

void DialogueSystem::ShowNode(const DialogueNode& node) const {
  ClearScreen();
  std::cout << "\n" << kDlgLine << "\n";
  if (!node.speaker.empty())
    std::cout << "  " << node.speaker << ":\n";
  std::cout << "  " << node.text << "\n";
  std::cout << kDlgLine << "\n";
}

int DialogueSystem::AskChoice(const std::vector<DialogueChoiceData>& choices) const {
  if (choices.size() == 1) {
    std::cout << "\n  [Нажми Enter...]\n  > ";
    std::cin.ignore(10000, '\n');
    return 0;
  }

  for (int i = 0; i < static_cast<int>(choices.size()); ++i)
    std::cout << "  " << (i + 1) << ". " << choices[i].choice_text << "\n";

  return ReadInt(1, static_cast<int>(choices.size())) - 1;
}

int DialogueSystem::ReadInt(int min_val, int max_val) const {
  int value;
  while (true) {
    std::cout << "  > ";
    if (std::cin >> value && value >= min_val && value <= max_val)
      break;
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cout << "  Введи число от " << min_val << " до " << max_val << ".\n";
  }
  std::cin.ignore(10000, '\n');
  return value;
}
