#pragma once
#include "data/GameDatabase.h"
#include "core/Hero.h"
#include "items/Inventory.h"
#include "systems/BattleSystem.h"
#include "systems/DialogueSystem.h"
#include "world/Castle.h"
#include "commerce/Merchant.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class Game {
public:
  explicit Game(const std::string& data_dir);
  void Run();

private:
  void LoadData();
  void InitHero();

  void ShowIntro();
  void PlayRoadSegment(const std::string& location, const std::string& road_text_key);
  void RunCastle(int castle_id);
  void RunRoom(Room& room);
  void HandleFinalApproach();
  void ShowVictory();
  void ShowDefeat();

  void TriggerLocationEvent(const std::string& location);
  void RunMerchantEncounter(const MerchantData& merchant_data);
  Merchant BuildMerchant(const MerchantData& md) const;

  void ApplyDialogueEffect(const std::string& effect_type, const std::string& effect_value);
  void GiveItemToHero(const std::string& item_spec);

  void RunRoomSearch(Room& room);

  void OfferShortcutToRyota();

  void Print(const std::string& text)      const;
  void PrintSep()                          const;
  void PrintLine()                         const;
  void PressEnter()                        const;
  int  ReadInt(int min_val, int max_val)   const;

  std::string data_dir_;

  GameDatabase                  db_;
  std::unique_ptr<Hero>         hero_;
  std::unique_ptr<Inventory>    inventory_;
  std::unique_ptr<BattleSystem>    battle_system_;
  std::unique_ptr<DialogueSystem>  dialogue_system_;

  std::map<std::string, bool> flags_;
  bool game_over_                  = false;
  bool player_won_                 = false;
  int  hints_found_                = 0;
  bool skip_to_ryota_              = false;
  bool finish_castle_then_ryota_   = false;
  bool shortcut_offered_           = false;
};
