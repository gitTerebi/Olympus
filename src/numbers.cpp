#include "numbers.h"

#include "game-dir.h"
#include "load-text-helper.h"

#include <iostream>

int Numbers::sDayLength;

double Numbers::sEliteHousingTaxMultiplier;
double Numbers::sCommonHousingTaxMulitplier;

double Numbers::sWageMultiplier;

int Numbers::sReinforcementsTravelTime;
int Numbers::sArmyTravelTime;
int Numbers::sAIInvasionMonthsBreak;
int Numbers::sInvasionAppearAtPlaces;
double Numbers::sArmyStrengthHorsemanMult;
double Numbers::sArmyStrengthTriremeMult;

int Numbers::sRabbleDivisor;
int Numbers::sSoldiersPerBanner;
int Numbers::sSoldiersPerAresArtemisBanner;

int Numbers::sEarthquakeProgressPeriod;

int Numbers::sBoarMaxCount;
int Numbers::sBoarSpawnPeriod;
int Numbers::sDeerMaxCount;
int Numbers::sDeerSpawnPeriod;
int Numbers::sWolfMaxCount;
int Numbers::sWolfSpawnPeriod;
int Numbers::sWolfHuntWait;
int Numbers::sWolfHuntDistance;

int Numbers::sOlivePressProcessingPeriod;
int Numbers::sWineryProcessingPeriod;
int Numbers::sArmoryProcessingPeriod;
int Numbers::sSculptureStudioProcessingPeriod;

int Numbers::sFarmRipePeriod;

int Numbers::sShepherdGoatherdMaxDistance;
int Numbers::sShepherdGoatherdMaxGroom;
int Numbers::sShepherdGoatherdGroomTime;
int Numbers::sShepherdGoatherdCollectTime;
int Numbers::sShepherdGoatherdWaitTime;

int Numbers::sSheepMaxGroom;
int Numbers::sGoatMaxGroom;
int Numbers::sAnimalMoveRange;

int Numbers::sCorralProcessingPeriod;
int Numbers::sCorralTakePeriod;
int Numbers::sCorralKillPeriod;
int Numbers::sCorralReplacePeriod;
int Numbers::sCattleMaturePeriod;

int Numbers::sUrchinQuayUnpackTime;
int Numbers::sFisheryBoatBuildTime;
int Numbers::sFisheryUnpackTime;

int Numbers::sHuntingLodgeWaitPeriod;

int Numbers::sMintDrachmasPerSilver;

int Numbers::sSpreadFirePeriod;
int Numbers::sFireCollapsePeriod;
int Numbers::sRuinsFireEndPeriod;
int Numbers::sMaintenanceDecrementPeriod;
double Numbers::sFireRiskPeriodMultiplier;
double Numbers::sFireRiskPeriodBaseIncrement;
double Numbers::sFireRiskPeriodExponent;
double Numbers::sCollapseRiskPeriodMultiplier;
double Numbers::sCollapseRiskPeriodBaseIncrement;
double Numbers::sCollapseRiskPeriodExponent;
int Numbers::sFireRiskPeriodTable[101];
int Numbers::sCollapseRiskPeriodTable[101];

int Numbers::sHouseCultureDecrementPeriod;
int Numbers::sHouseWaterDecrementPeriod;
int Numbers::sHouseHygieneDecrementPeriod;
double Numbers::sHouseHealPlaguePeriodMultiplier;
double Numbers::sHouseHealPlaguePeriodBaseMultiplier;
double Numbers::sHouseHealPlaguePeriodExponent;
double Numbers::sHousePlagueRiskPeriodMultiplier;
double Numbers::sHousePlagueRiskPeriodBaseIncrement;
double Numbers::sHousePlagueRiskPeriodExponent;
int Numbers::sHouseDisgruntledRemovePeriod;
double Numbers::sHouseDisgruntledRiskPeriodMultiplier;
double Numbers::sHouseDisgruntledRiskPeriodBaseIncrement;
double Numbers::sHouseDisgruntledRiskPeriodExponent;
double Numbers::sHouseLeaveRiskPeriodMultiplier;
double Numbers::sHouseLeaveRiskPeriodBaseIncrement;
double Numbers::sHouseLeaveRiskPeriodExponent;
int Numbers::sHouseSatisfactionUpdatePeriod;
int Numbers::sHouseDisgruntledSpawnPeriod;
int Numbers::sHouseSickSpawnPeriod;

int Numbers::sHerosHallArrivalPeriod;
int Numbers::sHerosHallSpawnPeriod;

int Numbers::sHorseRanchWheatUsePeriod;
int Numbers::sHorseRanchHorseSpawnPeriod;

int Numbers::sPatrolerMaxDistance;
int Numbers::sPatrolWalkerSpawnCooldown;
int Numbers::sWaterCarrierSpawnCooldown;
int Numbers::sInfirmarySpawnCooldown;
int Numbers::sScheduledWalkerSpawnCooldown;
int Numbers::sDestinationWalkerRecurringSpawnDays;
int Numbers::sDestinationWalkerInitialSpawnWaitDays;
int Numbers::sCulturePatrolSpawnCooldownDays;
int Numbers::sCultureShowDays;

int Numbers::sArtisanWaitTime;
int Numbers::sArtisanBuildTime;

int Numbers::sTowerSpawnPeriod;

int Numbers::sTraderSpawnPeriod;
int Numbers::sTwoWayTradeMax;

int Numbers::sChariotBuildingTime;

int Numbers::sOliveTreeRipePeriod;
int Numbers::sVineRipePeriod;
int Numbers::sOrangeTreeRipePeriod;

double Numbers::sTreeVineFullyRipePeriodMultiplier;

double Numbers::sTreeVineBlessedFullyRipePeriodMultiplier;
double Numbers::sTreeVineBlessedRipePeriodMultiplier;
double Numbers::sTreeVineCursedFullyRipePeriodMultiplier;
double Numbers::sTreeVineCursedRipePeriodMultiplier;

int Numbers::sMarbleCollectTime;
int Numbers::sBronzeCollectTime;
int Numbers::sSilverCollectTime;
int Numbers::sWoodCollectTime;
int Numbers::sFishCollectTime;
int Numbers::sUrchinCollectTime;

int Numbers::sGrowerMaxDistance;
int Numbers::sGrowerMaxGroom;
int Numbers::sGrowerWorkTime;
int Numbers::sGrowerSpawnWaitTime;

int Numbers::sMonsterAttackRange;

int Numbers::sPassiveMonsterAttackPeriod;
int Numbers::sActiveMonsterAttackPeriod;
int Numbers::sVeryActiveMonsterAttackPeriod;
int Numbers::sAggressiveMonsterAttackPeriod;

int Numbers::sPassiveLandMonsterInvadePeriod;
int Numbers::sActiveLandMonsterInvadePeriod;
int Numbers::sVeryActiveLandMonsterInvadePeriod;
int Numbers::sAggressiveLandMonsterInvadePeriod;

int Numbers::sPassiveWaterMonsterInvadePeriod;
int Numbers::sActiveWaterMonsterInvadePeriod;
int Numbers::sVeryActiveWaterMonsterInvadePeriod;
int Numbers::sAggressiveWaterMonsterInvadePeriod;

int Numbers::sPassiveLandMonsterMoveAroundPeriod;
int Numbers::sActiveLandMonsterMoveAroundPeriod;
int Numbers::sVeryActiveLandMonsterMoveAroundPeriod;
int Numbers::sAggressiveLandMonsterMoveAroundPeriod;

int Numbers::sPassiveWaterMonsterMoveAroundPeriod;
int Numbers::sActiveWaterMonsterMoveAroundPeriod;
int Numbers::sVeryActiveWaterMonsterMoveAroundPeriod;
int Numbers::sAggressiveWaterMonsterMoveAroundPeriod;

int Numbers::sFriendlyGodVisitPeriod;
int Numbers::sGodHelpPeriod;
int Numbers::sGodHelpAttackPeriod;
int Numbers::sGodHelpAttackPlayerPeriod;

int Numbers::sGodAttackCursePeriod;
int Numbers::sGodAttackCurseRange;
int Numbers::sGodAttackAttackPeriod;
int Numbers::sGodAttackAttackRange;
int Numbers::sGodAttackAggressiveAttackPeriod;
int Numbers::sGodAttackTargetedCursePeriod;
int Numbers::sGodAttackTargetedCurseRange;
int Numbers::sGodAttackTargetedAttackPeriod;
int Numbers::sGodAttackTargetedAttackRange;
int Numbers::sGodAttackGodFightRange;
int Numbers::sGodAttackApolloPlaguePeriod;
int Numbers::sGodAttackApolloPlagueRange;
int Numbers::sGodAttackAphroditeEvictPeriod;
int Numbers::sGodAttackAphroditeEvictRange;

int Numbers::sGodVisitSoldierAttackPeriod;
int Numbers::sGodVisitSoldierAttackRange;
int Numbers::sGodVisitBlessPeriod;
int Numbers::sGodVisitBlessRange;
int Numbers::sGodVisitPatrolDistance;
int Numbers::sGodVisitMoveAroundTime;

int Numbers::sGodWorshippedSoldierAttackPeriod;
int Numbers::sGodWorshippedSoldierAttackRange;
int Numbers::sGodWorshippedBlessPeriod;
int Numbers::sGodWorshippedBlessRange;

int Numbers::sSoldierBeingAttackedCallRange;
int Numbers::sInvasionEngageDefenderRange;

int Numbers::sTowerHP;
int Numbers::sTowerRange;
double Numbers::sTowerAttack;

int Numbers::sWallHP;

int Numbers::sWallArcherRange;
double Numbers::sWallArcherAttack;

int Numbers::sRabbleHP;
int Numbers::sRabbleRange;
double Numbers::sRabbleAttack;

int Numbers::sSpearthrowerHP;
int Numbers::sSpearthrowerRange;
double Numbers::sSpearthrowerAttack;

int Numbers::sArcherHP;
int Numbers::sArcherRange;
double Numbers::sArcherAttack;

int Numbers::sHopliteHP;
double Numbers::sHopliteAttack;

int Numbers::sChariotHP;
double Numbers::sChariotAttack;

int Numbers::sHorsemanHP;
double Numbers::sHorsemanAttack;

int Numbers::sWatchmanHP;
double Numbers::sWatchmanAttack;
int Numbers::sWatchmanSatisfactionProvide;

int Numbers::sDefendCityMaxKilled;
int Numbers::sDefendCityTalosMaxKilled;

int Numbers::sAttackCityMaxKilled;
int Numbers::sAttackCityAresMaxKilled;

int Numbers::sBasicVendorMaxResourceTakeDistance;
int Numbers::sHorseVendorMaxResourceTakeDistance;
int Numbers::sResourceBuildingMaxResourceGiveDistance;
int Numbers::sProcessingBuildingMaxResourceTakeDistance;
int Numbers::sTriremeWharfMaxResourceTakeDistance;

int Numbers::sTriremeWharfBuildTime;
int Numbers::sTriremeWharfBuildStages;

int Numbers::sNewBuildingEmployWaitDays;

int Numbers::sSacrificeDurationDays;
int Numbers::sPriestSacrificeRecurringSpawnDays;

void Numbers::sLoad() {
    const auto path = GameDir::numbersPath();
    sLoad(path);
}

void Numbers::sLoad(const std::string& path) {
    std::map<std::string, std::string> map;
    LoadTextHelper::load(path, map);

    const bool missing = map.empty();

    const auto loadI = [&](const std::string& name,
                           int& val, const int def,
                           const int min, const int max) {
        if(missing) {
            val = def;
            return;
        }
        const auto it = map.find(name);
        if(it == map.end()) {
            val = def;
            return;
        }
        const auto iStr = it->second;
        try {
            val = std::stoi(iStr);
            if(val > max) {
                val = max;
            } else if(val < min) {
                val = min;
            }
        } catch(...) {
            val = def;
            return;
        }
    };

    const auto loadD = [&](const std::string& name,
                           double& val, const double def,
                           const double min, const double max) {
        if(missing) {
            val = def;
            return;
        }
        const auto it = map.find(name);
        if(it == map.end()) {
            val = def;
            return;
        }
        const auto iStr = it->second;
        try {
            val = std::stod(iStr);
            if(val > max) {
                val = max;
            } else if(val < min) {
                val = min;
            }
        } catch(...) {
            val = def;
            return;
        }
    };
    loadI("day_length_i", sDayLength, 350, 1, 10000);

    loadD("elite_housing_tax_mult_d", sEliteHousingTaxMultiplier, 1, 0, 10);
    loadD("common_housing_tax_mult_d", sCommonHousingTaxMulitplier, 1, 0, 10);
    loadD("wage_mult_d", sWageMultiplier, 1, 0, 10);

    loadI("reinforcements_travel_time_i", sReinforcementsTravelTime, 50, 1, 1000);
    loadI("army_travel_time_i", sArmyTravelTime, 200, 1, 1000);
    loadI("ai_invasion_months_break_i", sAIInvasionMonthsBreak, 30, 0, 1000);
    loadI("invasion_appear_at_places_b", sInvasionAppearAtPlaces, 0, 0, 1);
    loadD("army_strength_horseman_mult_d", sArmyStrengthHorsemanMult, 1.5, 0.1, 100.);
    loadD("army_strength_trireme_mult_d", sArmyStrengthTriremeMult, 8., 0.1, 1000.);

    loadI("rabble_divisor_i", sRabbleDivisor, 6, 1, 100);
    loadI("soldiers_per_banner_i", sSoldiersPerBanner, 8, 2, 100);
    loadI("soldiers_per_ares_artemis_banner_i", sSoldiersPerAresArtemisBanner, 8, 2, 100);

    loadI("earthquake_progress_period_i", sEarthquakeProgressPeriod, 500, 1, 10000);

    loadI("boar_max_count_i", sBoarMaxCount, 8, 2, 100);
    loadI("boar_spawn_period_i", sBoarSpawnPeriod, 4000, 1, 100000);
    loadI("deer_max_count_i", sDeerMaxCount, 8, 2, 100);
    loadI("deer_spawn_period_i", sDeerSpawnPeriod, 4000, 1, 100000);
    loadI("wolf_max_count_i", sWolfMaxCount, 8, 2, 100);
    loadI("wolf_spawn_period_i", sWolfSpawnPeriod, 4000, 1, 100000);
    loadI("wolf_hunt_wait_i", sWolfHuntWait, 20, 1, 1000);
    loadI("wolf_hunt_distance_i", sWolfHuntDistance, 15, 1, 1000);

    loadI("olive_press_processing_period_i", sOlivePressProcessingPeriod, 20000, 1, 1000000);
    loadI("winery_processing_period_i", sWineryProcessingPeriod, 20000, 1, 1000000);
    loadI("armory_processing_period_i", sArmoryProcessingPeriod, 21000, 1, 1000000);
    loadI("sculpture_studio_processing_period_i", sSculptureStudioProcessingPeriod, 21000, 1, 1000000);

    loadI("farm_ripe_period_i", sFarmRipePeriod, 25550, 1, 1000000);

    loadI("shepherd_goatherd_max_distance_i", sShepherdGoatherdMaxDistance, 40, 1, 1000);
    loadI("shepherd_goatherd_max_groom_i", sShepherdGoatherdMaxGroom, 20, 1, 1000);
    loadI("shepherd_goatherd_groom_time_i", sShepherdGoatherdGroomTime, 1000, 1, 10000);
    loadI("shepherd_goatherd_collect_time_i", sShepherdGoatherdCollectTime, 2000, 1, 10000);
    loadI("shepherd_goatherd_wait_time_i", sShepherdGoatherdWaitTime, 2000, 1, 100000);

    loadI("sheep_max_groom_i", sSheepMaxGroom, 15, 1, 1000);
    loadI("goat_max_groom_i", sGoatMaxGroom, 10, 1, 1000);
    loadI("animal_move_range_i", sAnimalMoveRange, 5, 2, 100);

    loadI("corral_processing_period_i", sCorralProcessingPeriod, 10000, 1, 1000000);
    loadI("corral_take_period_i", sCorralTakePeriod, 10000, 1, 1000000);
    loadI("corral_kill_period_i", sCorralKillPeriod, 10000, 1, 1000000);
    loadI("corral_replace_period_i", sCorralReplacePeriod, 10000, 1, 1000000);
    loadI("cattle_mature_period_i", sCattleMaturePeriod, 25000, 1, 1000000);

    loadI("urchin_quay_unpack_time_i", sUrchinQuayUnpackTime, 10000, 1, 1000000);
    loadI("fishery_boat_build_time_i", sFisheryBoatBuildTime, 40000, 1, 1000000);
    loadI("fishery_unpack_time_i", sFisheryUnpackTime, 10000, 1, 1000000);

    loadI("hunting_lodge_wait_period_i", sHuntingLodgeWaitPeriod, 5000, 1, 1000000);

    loadI("mint_drachmas_per_silver_i", sMintDrachmasPerSilver, 50, 0, 1000);

    loadI("spread_fire_period_i", sSpreadFirePeriod, 50000, 1, 1000000);
    loadI("fire_collapse_period_i", sFireCollapsePeriod, 50000, 1, 1000000);
    loadI("ruins_fire_end_period_i", sRuinsFireEndPeriod, 10000, 1, 1000000);
    loadI("maintenance_decrement_period_i", sMaintenanceDecrementPeriod, 2000, 1, 1000000);
    loadD("fire_risk_period_multiplier_d", sFireRiskPeriodMultiplier, 10, 1, 100);
    loadD("fire_risk_period_base_increment_d", sFireRiskPeriodBaseIncrement, 10, 1, 1000);
    loadD("fire_risk_period_exponent_d", sFireRiskPeriodExponent, 4, 1, 9);
    loadD("collapse_risk_period_multiplier_d", sCollapseRiskPeriodMultiplier, 10, 1, 100);
    loadD("collapse_risk_period_base_increment_d", sCollapseRiskPeriodBaseIncrement, 10, 1, 1000);
    loadD("collapse_risk_period_exponent_d", sCollapseRiskPeriodExponent, 4, 1, 9);
    for(int m = 0; m <= 100; m++) {
        sFireRiskPeriodTable[m] = static_cast<int>(sFireRiskPeriodMultiplier * pow(sFireRiskPeriodBaseIncrement + m, sFireRiskPeriodExponent));
        sCollapseRiskPeriodTable[m] = static_cast<int>(sCollapseRiskPeriodMultiplier * pow(sCollapseRiskPeriodBaseIncrement + m, sCollapseRiskPeriodExponent));
    }

    loadI("house_culture_decrement_period_i", sHouseCultureDecrementPeriod, 20000, 1, 1000000);
    loadI("house_water_decrement_period_i", sHouseWaterDecrementPeriod, 7500, 1, 1000000);
    loadI("house_hygiene_decrement_period_i", sHouseHygieneDecrementPeriod, 25000, 1, 1000000);
    loadD("house_heal_plague_period_multiplier_d", sHouseHealPlaguePeriodMultiplier, 10, 1, 100);
    loadD("house_heal_plague_period_base_multiplier_d", sHouseHealPlaguePeriodBaseMultiplier, 1000, 1, 100000);
    loadD("house_heal_plague_period_exponent_d", sHouseHealPlaguePeriodExponent, 4, 1, 9);
    loadD("house_plague_risk_period_multiplier_d", sHousePlagueRiskPeriodMultiplier, 10, 1, 100);
    loadD("house_plague_risk_period_base_increment_d", sHousePlagueRiskPeriodBaseIncrement, 10, 1, 100);
    loadD("house_plague_risk_period_exponent_d", sHousePlagueRiskPeriodExponent, 4, 1, 9);
    loadI("house_disgruntled_remove_period_i", sHouseDisgruntledRemovePeriod, 500000, 1, 1000000);
    loadD("house_disgruntled_risk_period_multiplier_d", sHouseDisgruntledRiskPeriodMultiplier, 10, 1, 100);
    loadD("house_disgruntled_risk_period_base_increment_d", sHouseDisgruntledRiskPeriodBaseIncrement, 10, 1, 100);
    loadD("house_disgruntled_risk_period_exponent_d", sHouseDisgruntledRiskPeriodExponent, 3, 1, 9);
    loadD("house_leave_risk_period_multiplier_d", sHouseLeaveRiskPeriodMultiplier, 10, 1, 100);
    loadD("house_leave_risk_period_base_increment_d", sHouseLeaveRiskPeriodBaseIncrement, 10, 1, 100);
    loadD("house_leave_risk_period_exponent_d", sHouseLeaveRiskPeriodExponent, 3, 1, 9);
    loadI("house_satisfaction_update_period_i", sHouseSatisfactionUpdatePeriod, 7500, 1, 1000000);
    loadI("house_disgruntled_spawn_period_i", sHouseDisgruntledSpawnPeriod, 10000, 1, 1000000);
    loadI("house_sick_spawn_period_i", sHouseSickSpawnPeriod, 10000, 1, 1000000);

    loadI("heros_hall_arrival_period_i", sHerosHallArrivalPeriod, 15000, 1, 1000000);
    loadI("heros_hall_spawn_period_i", sHerosHallSpawnPeriod, 5000, 1, 1000000);

    loadI("horse_ranch_wheat_use_period_i", sHorseRanchWheatUsePeriod, 1000, 1, 1000000);
    loadI("horse_ranch_horse_spawn_period_i", sHorseRanchHorseSpawnPeriod, 21000, 1, 1000000);

    loadI("patroler_max_distance_i", sPatrolerMaxDistance, 40, 5, 1000);
    loadI("patrol_walker_spawn_cooldown_i", sPatrolWalkerSpawnCooldown, 1000, 0, 1000000);

    loadI("water_carrier_spawn_cooldown_i", sWaterCarrierSpawnCooldown, 4000, 0, 1000000);
    loadI("infirmary_spawn_cooldown_i", sInfirmarySpawnCooldown, 1000, 0, 1000000);
    loadI("scheduled_walker_spawn_cooldown_i", sScheduledWalkerSpawnCooldown, 2000, 1, 1000000);

    loadI("destination_walker_spawn_interval_days_i", sDestinationWalkerRecurringSpawnDays, 32, 1, 10000);
    loadI("destination_walker_initial_spawn_wait_days_i", sDestinationWalkerInitialSpawnWaitDays, 16, 1, 10000);
    loadI("culture_patrol_spawn_cooldown_days_i", sCulturePatrolSpawnCooldownDays, 18, 0, 10000);
    loadI("culture_show_days_i", sCultureShowDays, 64, 0, 100000);

    loadI("artisan_wait_time_i", sArtisanWaitTime, 2000, 1, 1000000);
    loadI("artisan_build_time_i", sArtisanBuildTime, 6000, 1, 1000000);

    loadI("tower_spawn_period_i", sTowerSpawnPeriod, 5000, 1, 1000000);

    loadI("trader_spawn_period_i", sTraderSpawnPeriod, 30000, 1, 1000000);
    loadI("two_way_trade_max_i", sTwoWayTradeMax, 4, 0, 100);

    loadI("chariot_building_time_i", sChariotBuildingTime, 20000, 1, 1000000);

    loadI("olive_tree_ripe_period_i", sOliveTreeRipePeriod, 12600, 1, 1000000);
    loadI("vine_ripe_period_i", sVineRipePeriod, 40000, 1, 1000000);
    loadI("orange_tree_ripe_period_i", sOrangeTreeRipePeriod, 40000, 1, 1000000);

    loadD("tree_vine_fully_ripe_period_multiplier_d", sTreeVineFullyRipePeriodMultiplier, 2., 1, 9);

    loadD("tree_vine_blessed_fully_ripe_period_multiplier_d", sTreeVineBlessedFullyRipePeriodMultiplier, 3., 1, 9);
    loadD("tree_vine_blessed_ripe_period_multiplier_d", sTreeVineBlessedRipePeriodMultiplier, 0.333, 0.1, 1.);
    loadD("tree_vine_cursed_fully_ripe_period_multiplier_d", sTreeVineCursedFullyRipePeriodMultiplier, 0.333, 0.1, 1.);
    loadD("tree_vine_cursed_ripe_period_multiplier_d", sTreeVineCursedRipePeriodMultiplier, 3., 1, 9);

    loadI("marble_collect_period_i", sMarbleCollectTime, 9800, 1, 1000000);
    loadI("bronze_collect_period_i", sBronzeCollectTime, 8500, 1, 1000000);
    loadI("silver_collect_period_i", sSilverCollectTime, 8500, 1, 1000000);
    loadI("wood_collect_period_i", sWoodCollectTime, 8500, 1, 1000000);
    loadI("fish_collect_period_i", sFishCollectTime, 29000, 1, 1000000);
    loadI("urchin_collect_period_i", sUrchinCollectTime, 29000, 1, 1000000);

    loadI("grower_max_distance_i", sGrowerMaxDistance, 40, 5, 1000);
    loadI("grower_max_groom_i", sGrowerMaxGroom, 5, 1, 1000);
    loadI("grower_work_time_i", sGrowerWorkTime, 2000, 1, 1000000);
    loadI("grower_spawn_wait_time_i", sGrowerSpawnWaitTime, 1500, 1, 1000000);

    loadI("monster_attack_range_i", sMonsterAttackRange, 10, 1, 20);
    loadI("passive_monster_attack_period_i", sPassiveMonsterAttackPeriod, 6000, 1, 1000000);
    loadI("active_monster_attack_period_i", sActiveMonsterAttackPeriod, 4000, 1, 1000000);
    loadI("very_active_monster_attack_period_i", sVeryActiveMonsterAttackPeriod, 3000, 1, 1000000);
    loadI("aggressive_monster_attack_period_i", sAggressiveMonsterAttackPeriod, 2000, 1, 1000000);

    loadI("passive_land_monster_invade_period_i", sPassiveLandMonsterInvadePeriod, 1000000, 1, 1000000);
    loadI("active_land_monster_invade_period_i", sActiveLandMonsterInvadePeriod, 600000, 1, 1000000);
    loadI("very_active_land_monster_invade_period_i", sVeryActiveLandMonsterInvadePeriod, 400000, 1, 1000000);
    loadI("aggressive_land_monster_invade_period_i", sAggressiveLandMonsterInvadePeriod, 200000, 1, 1000000);

    loadI("passive_water_monster_invade_period_i", sPassiveWaterMonsterInvadePeriod, 800000, 1, 1000000);
    loadI("active_water_monster_invade_period_i", sActiveWaterMonsterInvadePeriod, 400000, 1, 1000000);
    loadI("very_active_water_monster_invade_period_i", sVeryActiveWaterMonsterInvadePeriod, 200000, 1, 1000000);
    loadI("aggressive_water_monster_invade_period_i", sAggressiveWaterMonsterInvadePeriod, 100000, 1, 1000000);

    loadI("passive_land_monster_move_around_period_i", sPassiveLandMonsterMoveAroundPeriod, 20000, 1, 1000000);
    loadI("active_land_monster_move_around_period_i", sActiveLandMonsterMoveAroundPeriod, 30000, 1, 1000000);
    loadI("very_active_land_monster_move_around_period_i", sVeryActiveLandMonsterMoveAroundPeriod, 40000, 1, 1000000);
    loadI("aggressive_land_monster_move_around_period_i", sAggressiveLandMonsterMoveAroundPeriod, 50000, 1, 1000000);

    loadI("passive_water_monster_move_around_period_i", sPassiveWaterMonsterMoveAroundPeriod, 20000, 1, 1000000);
    loadI("active_water_monster_move_around_period_i", sActiveWaterMonsterMoveAroundPeriod, 30000, 1, 1000000);
    loadI("very_active_water_monster_move_around_period_i", sVeryActiveWaterMonsterMoveAroundPeriod, 40000, 1, 1000000);
    loadI("aggressive_water_monster_move_around_period_i", sAggressiveWaterMonsterMoveAroundPeriod, 50000, 1, 1000000);

    loadI("friendly_god_visit_period_i", sFriendlyGodVisitPeriod, 450, 1, 10000);
    loadI("god_help_period_i", sGodHelpPeriod, 250000, 1, 1000000);
    loadI("god_help_attack_period_i", sGodHelpAttackPeriod, 500000, 1, 1000000);
    loadI("god_help_attack_player_period_i", sGodHelpAttackPlayerPeriod, 250000, 1, 1000000);

    loadI("god_attack_curse_period_i", sGodAttackCursePeriod, 6000, 1, 1000000);
    loadI("god_attack_curse_range_i", sGodAttackCurseRange, 10, 1, 20);
    loadI("god_attack_attack_period_i", sGodAttackAttackPeriod, 12000, 1, 1000000);
    loadI("god_attack_attack_range_i", sGodAttackAttackRange, 10, 1, 20);
    loadI("god_attack_aggressive_attack_period_i", sGodAttackAggressiveAttackPeriod, 8000, 1, 1000000);
    loadI("god_attack_targeted_curse_period_i", sGodAttackTargetedCursePeriod, 3000, 1, 1000000);
    loadI("god_attack_targeted_curse_range_i", sGodAttackTargetedCurseRange, 10, 1, 20);
    loadI("god_attack_targeted_attack_period_i", sGodAttackTargetedAttackPeriod, 4000, 1, 1000000);
    loadI("god_attack_targeted_attack_range_i", sGodAttackTargetedAttackRange, 10, 1, 20);
    loadI("god_attack_god_fight_range_i", sGodAttackGodFightRange, 10, 1, 20);
    loadI("god_attack_apollo_plague_period_i", sGodAttackApolloPlaguePeriod, 7500, 1, 1000000);
    loadI("god_attack_apollo_plague_range_i", sGodAttackApolloPlagueRange, 10, 1, 20);
    loadI("god_attack_aphrodite_evict_period_i", sGodAttackAphroditeEvictPeriod, 3500, 1, 1000000);
    loadI("god_attack_aphrodite_evict_range_i", sGodAttackAphroditeEvictRange, 10, 1, 20);

    loadI("god_visit_soldier_attack_period_i", sGodVisitSoldierAttackPeriod, 2500, 1, 1000000);
    loadI("god_visit_soldier_attack_range_i", sGodVisitSoldierAttackRange, 10, 1, 20);
    loadI("god_visit_bless_period_i", sGodVisitBlessPeriod, 18000, 1, 1000000);
    loadI("god_visit_bless_range_i", sGodVisitBlessRange, 10, 1, 20);
    loadI("god_visit_patrol_distance_i", sGodVisitPatrolDistance, 100, 10, 1000);
    loadI("god_visit_move_around_time_i", sGodVisitMoveAroundTime, 25000, 1, 1000000);

    loadI("god_worshipped_soldier_attack_period_i", sGodWorshippedSoldierAttackPeriod, 2500, 1, 1000000);
    loadI("god_worshipped_soldier_attack_range_i", sGodWorshippedSoldierAttackRange, 10, 1, 20);
    loadI("god_worshipped_bless_period_i", sGodWorshippedBlessPeriod, 6000, 1, 1000000);
    loadI("god_worshipped_bless_range_i", sGodWorshippedBlessRange, 10, 1, 20);

    loadI("soldier_being_attacked_call_range_i", sSoldierBeingAttackedCallRange, 2, 0, 100);

    loadI("invasion_engage_defender_range_i", sInvasionEngageDefenderRange, 15, 0, 200);

    loadI("tower_hp_i", sTowerHP, 5000, 1, 1000000);
    loadI("tower_range_i", sTowerRange, 8, 2, 100);
    loadD("tower_attack_d", sTowerAttack, 0.25, 0.1, 10.);

    loadI("wall_hp_i", sWallHP, 2000, 1, 1000000);

    loadI("wall_archer_range_i", sWallArcherRange, 8, 2, 200);
    loadD("wall_archer_attack_d", sWallArcherAttack, 0.25, 0.1, 10.);

    loadI("rabble_hp_i", sRabbleHP, 300, 1, 1000000);
    loadI("rabble_range_i", sRabbleRange, 4, 1, 20);
    loadD("rabble_attack_d", sRabbleAttack, 0.05, 0.01, 10);

    loadI("spearthrower_hp_i", sSpearthrowerHP, 300, 1, 1000000);
    loadI("spearthrower_range_i", sSpearthrowerRange, 4, 1, 20);
    loadD("spearthrower_attack_d", sSpearthrowerAttack, 0.05, 0.01, 10);

    loadI("archer_hp_i", sArcherHP, 300, 1, 1000000);
    loadI("archer_range_i", sArcherRange, 4, 1, 20);
    loadD("archer_attack_d", sArcherAttack, 0.05, 0.01, 10);

    loadI("hoplite_hp_i", sHopliteHP, 500, 1, 1000000);
    loadD("hoplite_attack_d", sHopliteAttack, 0.5, 0.01, 10);

    loadI("chariot_hp_i", sChariotHP, 500, 1, 1000000);
    loadD("chariot_attack_d", sChariotAttack, 0.6, 0.01, 10);

    loadI("horseman_hp_i", sHorsemanHP, 500, 1, 1000000);
    loadD("horseman_attack_d", sHorsemanAttack, 0.6, 0.01, 10);

    loadI("watchman_hp_i", sWatchmanHP, 200, 1, 1000000);
    loadD("watchman_attack_d", sWatchmanAttack, 0.5, 0.01, 10);
    loadI("watchman_satisfaction_provide_i", sWatchmanSatisfactionProvide, 25, 0, 100);

    loadI("defend_city_max_killed_i", sDefendCityMaxKilled, 6, 1, 100);
    loadI("defend_city_talos_max_killed_i", sDefendCityTalosMaxKilled, 16, 1, 100);

    loadI("attack_city_max_killed_i", sAttackCityMaxKilled, 4, 1, 100);
    loadI("attack_city_ares_max_killed_i", sAttackCityAresMaxKilled, 8, 1, 100);

    loadI("basic_vendor_max_resource_take_distance_i", sBasicVendorMaxResourceTakeDistance, 999, 5, 1000);
    loadI("horse_vendor_max_resource_take_distance_i", sHorseVendorMaxResourceTakeDistance, 200, 5, 1000);

    loadI("resource_building_max_resource_give_distance_i", sResourceBuildingMaxResourceGiveDistance, 999, 5, 1000);
    loadI("processing_building_max_resource_take_distance_i", sProcessingBuildingMaxResourceTakeDistance, 999, 5, 1000);
    loadI("trireme_wharf_max_resource_take_distance_i", sTriremeWharfMaxResourceTakeDistance, 60, 5, 1000);
    loadI("trireme_wharf_build_time_i", sTriremeWharfBuildTime, 100000, 1, 1000000);
    loadI("trireme_wharf_build_stages_i", sTriremeWharfBuildStages, 3, 1, 10);

    loadI("new_building_employ_wait_days_i", sNewBuildingEmployWaitDays, 6, 0, 360);

    loadI("sacrifice_duration_days_i", sSacrificeDurationDays, 64, 1, 360);
    loadI("priest_sacrifice_recurring_spawn_days_i", sPriestSacrificeRecurringSpawnDays, 32, 1, 360);
}
