#include "echaracterinfowidget.h"

#include "characters/ecarttransporter.h"
#include "characters/egrower.h"
#include "characters/ehunter.h"
#include "widgets/elabel.h"
#include "textures/egametextures.h"
#include "elanguage.h"
#include "characters/gods/egod.h"
#include "characters/heroes/ehero.h"
#include "characters/monsters/emonster.h"
#include "characters/epeddler.h"
#include "buildings/eagorabase.h"
#include "widgets/ebuttonbase.h"
#include "widgets/ebasicbutton.h"
#include "engine/egameboard.h"
#include "audio/esounds.h"
#include "audio/esoundvector.h"
#include "eiteratesquare.h"

eCharacterInfoWidget::eCharacterInfoWidget(
    eMainWindow* const window,
    eMainWidget* const mw) :
    eInfoWidget(window, mw, true, true) {}

std::shared_ptr<eTexture> gCharPortrait(
    const eResolution res, const eCharacter* const c) {
    if(!c) return nullptr;
    const auto type = c->type();
    const auto uiScale = res.uiScale();
    const int iRes = static_cast<int>(uiScale);
    const auto& intrfc = eGameTextures::interface()[iRes];
    const auto& zp = intrfc.fZeusPortraits;
    const auto& zgp = intrfc.fZeusGodPortraits;
    const auto& pp = intrfc.fPoseidonPortraits;
    switch(type) {
    case eCharacterType::zeus:
        return zgp.getTexture(1);
    case eCharacterType::poseidon:
        return zgp.getTexture(2);
    case eCharacterType::demeter:
        return zgp.getTexture(3);
    case eCharacterType::apollo:
        return zgp.getTexture(4);
    case eCharacterType::artemis:
        return zgp.getTexture(5);
    case eCharacterType::ares:
        return zgp.getTexture(6);
    case eCharacterType::aphrodite:
        return zgp.getTexture(7);
    case eCharacterType::hermes:
        return zgp.getTexture(8);
    case eCharacterType::athena:
        return zgp.getTexture(9);
    case eCharacterType::hephaestus:
        return zgp.getTexture(10);
    case eCharacterType::dionysus:
        return zgp.getTexture(11);
    case eCharacterType::hades:
        return zgp.getTexture(12);

    case eCharacterType::achilles:
        return zgp.getTexture(13);
    case eCharacterType::hercules:
        return zgp.getTexture(14);
    case eCharacterType::jason:
        return zgp.getTexture(15);
    case eCharacterType::odysseus:
        return zgp.getTexture(16);
    case eCharacterType::perseus:
        return zgp.getTexture(17);
    case eCharacterType::theseus:
        return zgp.getTexture(18);

    case eCharacterType::hydra:
        return zgp.getTexture(19);
    case eCharacterType::kraken:
        return zgp.getTexture(20);
    case eCharacterType::cerberus:
        return zgp.getTexture(21);
    case eCharacterType::cyclops:
        return zgp.getTexture(22);
    case eCharacterType::minotaur:
        return zgp.getTexture(23);
    case eCharacterType::medusa:
        return zgp.getTexture(24);
    case eCharacterType::calydonianBoar:
        return zgp.getTexture(25);
    case eCharacterType::dragon:
        return zgp.getTexture(26);
    case eCharacterType::talos:
        return zgp.getTexture(27);
    case eCharacterType::hector:
        return zgp.getTexture(28);
    case eCharacterType::scylla:
        return zgp.getTexture(29);
    case eCharacterType::maenads:
        return zgp.getTexture(30);
    case eCharacterType::satyr:
        return zgp.getTexture(31);

    case eCharacterType::settler:
        return zp.getTexture(1);

    case eCharacterType::homeless:
        return zp.getTexture(3);

    case eCharacterType::grower: {
        const auto g = static_cast<const eGrower*>(c);
        const auto type = g->growerType();
        switch(type) {
        case eGrowerType::grapesAndOlives:
            return zp.getTexture(5);
        case eGrowerType::oranges:
            return pp.getTexture(2);
        }
    } break;

    case eCharacterType::taxCollector:
        return zp.getTexture(7);
    case eCharacterType::fireFighter:
        return zp.getTexture(8);
    case eCharacterType::cartTransporter: {
        const auto ct = static_cast<const eCartTransporter*>(c);
        const auto type = ct->cartType();
        switch(type) {
        case eCartTransporterType::basic:
            return zp.getTexture(9);
        case eCartTransporterType::food:
            return zp.getTexture(37);
        case eCartTransporterType::arms:
            return zp.getTexture(74);
        case eCartTransporterType::fleece:
            return zp.getTexture(75);
        case eCartTransporterType::oil:
            return zp.getTexture(76);
        case eCartTransporterType::wine:
            return zp.getTexture(77);
        case eCartTransporterType::horse:
            return zp.getTexture(78);
        case eCartTransporterType::ox:
            if(c->atlantean()) {
                return pp.getTexture(8);
            } else {
                return zp.getTexture(80);
            }
        case eCartTransporterType::chariot:
            return pp.getTexture(7);
        default:
            return zp.getTexture(0);
        }
    } break;

    case eCharacterType::rockThrower:
        return zp.getTexture(11);
    case eCharacterType::hoplite:
        return zp.getTexture(12);
    case eCharacterType::horseman:
        return zp.getTexture(13);

    case eCharacterType::competitor:
        return zp.getTexture(15);
    case eCharacterType::philosopher:
        return zp.getTexture(16);
    case eCharacterType::gymnast:
        return zp.getTexture(17);
    case eCharacterType::actor:
        return zp.getTexture(18);
    case eCharacterType::trader:
        return zp.getTexture(19);
    case eCharacterType::tradeBoat:
        return zp.getTexture(20);
    case eCharacterType::donkey:
        return zp.getTexture(21);

    case eCharacterType::disgruntled:
        return zp.getTexture(23);

    case eCharacterType::fishingBoat:
        return zp.getTexture(25);
    case eCharacterType::peddler:
        return zp.getTexture(26);

    case eCharacterType::shepherd:
        return zp.getTexture(30);
    case eCharacterType::goatherd:
        return zp.getTexture(31);
    case eCharacterType::healer:
        return zp.getTexture(32);

    case eCharacterType::eliteCitizen:
        return zp.getTexture(38);
    case eCharacterType::archer:
        return zp.getTexture(39);

    case eCharacterType::horse:
        return zp.getTexture(43);
    case eCharacterType::urchinGatherer:
        return zp.getTexture(44);

    case eCharacterType::sheep:
        return zp.getTexture(49);
    case eCharacterType::goat:
        return zp.getTexture(50);
    case eCharacterType::boar:
        return zp.getTexture(51);

    case eCharacterType::hunter: {
        const auto h = static_cast<const eHunter*>(c);
        const bool d = h->deerHunter();
        if(d) {
            return pp.getTexture(4);
        } else {
            return zp.getTexture(53);
        }
    } break;

    case eCharacterType::lumberjack:
        return zp.getTexture(55);

    case eCharacterType::trireme:
        return zp.getTexture(57);
    case eCharacterType::artisan:
        return zp.getTexture(58);
    case eCharacterType::wolf:
        return zp.getTexture(59);
    case eCharacterType::waterDistributor:
        return zp.getTexture(60);
    case eCharacterType::watchman:
        return zp.getTexture(61);

    case eCharacterType::sick:
        return zp.getTexture(66);

    case eCharacterType::silverMiner:
    case eCharacterType::marbleMiner:
        return zp.getTexture(72);
    case eCharacterType::bronzeMiner:
        return zp.getTexture(73);

    case eCharacterType::ox:
        return zp.getTexture(79);

    case eCharacterType::deer:
        return pp.getTexture(3);

    case eCharacterType::orichalcMiner:
        return pp.getTexture(5);

    case eCharacterType::scholar:
        return pp.getTexture(9);

    case eCharacterType::astronomer:
        return pp.getTexture(11);

    case eCharacterType::archerPoseidon:
        return pp.getTexture(14);
    case eCharacterType::hoplitePoseidon:
        return pp.getTexture(15);
    case eCharacterType::chariotPoseidon:
        return pp.getTexture(16);

    case eCharacterType::hera:
        return pp.getTexture(31);
    case eCharacterType::atlas:
        return pp.getTexture(32);

    case eCharacterType::chimera:
        return pp.getTexture(35);
    case eCharacterType::echidna:
        return pp.getTexture(36);
    case eCharacterType::harpies:
        return pp.getTexture(37);
    case eCharacterType::sphinx:
        return pp.getTexture(38);
    case eCharacterType::bellerophon:
        return pp.getTexture(39);
    case eCharacterType::atalanta:
        return pp.getTexture(40);

    case eCharacterType::inventor:
        return pp.getTexture(42);
    case eCharacterType::curator:
        return pp.getTexture(43);

    default:
        return zgp.getTexture(0);
    }
}

std::string gCharName(const eCharacterType c, const int seedId) {
    switch(c) {
    case eCharacterType::settler:
    case eCharacterType::homeless:
    case eCharacterType::sick:
    case eCharacterType::healer:
    case eCharacterType::disgruntled:
    case eCharacterType::actor:
    case eCharacterType::philosopher:
    case eCharacterType::competitor:
    case eCharacterType::gymnast:
    case eCharacterType::scholar:
    case eCharacterType::astronomer:
    case eCharacterType::cartTransporter:
    case eCharacterType::inventor:
    case eCharacterType::curator:
    case eCharacterType::rockThrower:
    case eCharacterType::hoplite:
    case eCharacterType::horseman:
    case eCharacterType::archerPoseidon:
    case eCharacterType::hoplitePoseidon:
    case eCharacterType::chariotPoseidon:
    case eCharacterType::fireFighter:
    case eCharacterType::taxCollector:
    case eCharacterType::watchman:
    case eCharacterType::waterDistributor:
    case eCharacterType::grower:
    case eCharacterType::bronzeMiner:
    case eCharacterType::marbleMiner:
    case eCharacterType::silverMiner:
    case eCharacterType::lumberjack:
        return eLanguage::zeusText(275, seedId % 16);
    case eCharacterType::aphrodite:
    case eCharacterType::apollo:
    case eCharacterType::ares:
    case eCharacterType::artemis:
    case eCharacterType::athena:
    case eCharacterType::atlas:
    case eCharacterType::demeter:
    case eCharacterType::dionysus:
    case eCharacterType::hades:
    case eCharacterType::hephaestus:
    case eCharacterType::hera:
    case eCharacterType::hermes:
    case eCharacterType::poseidon:
    case eCharacterType::zeus: {
        const auto g = eGod::sCharacterToGodType(c);
        return eGod::sGodName(g);
    } break;
    case eCharacterType::achilles:
    case eCharacterType::atalanta:
    case eCharacterType::bellerophon:
    case eCharacterType::hercules:
    case eCharacterType::jason:
    case eCharacterType::odysseus:
    case eCharacterType::perseus:
    case eCharacterType::theseus: {
        const auto h = eHero::sCharacterToHeroType(c);
        return eHero::sHeroName(h);
    } break;
    case eCharacterType::calydonianBoar:
    case eCharacterType::cerberus:
    case eCharacterType::chimera:
    case eCharacterType::cyclops:
    case eCharacterType::dragon:
    case eCharacterType::echidna:
    case eCharacterType::harpies:
    case eCharacterType::hector:
    case eCharacterType::hydra:
    case eCharacterType::kraken:
    case eCharacterType::maenads:
    case eCharacterType::medusa:
    case eCharacterType::minotaur:
    case eCharacterType::scylla:
    case eCharacterType::sphinx:
    case eCharacterType::talos:
    case eCharacterType::satyr: {
        const auto m = eMonster::sCharacterToMonsterType(c);
        return eMonster::sMonsterName(m);
    } break;
    default:
        return "";
    }
}

std::string gCharOccupation(
    const eCharacter* const c) {
    if(!c) return nullptr;
    auto& board = c->getBoard();
    const auto cid = c->cityId();
    const auto cname = board.cityName(cid);
    const auto type = c->type();
    switch(type) {
    case eCharacterType::aphrodite:
    case eCharacterType::apollo:
    case eCharacterType::ares:
    case eCharacterType::artemis:
    case eCharacterType::athena:
    case eCharacterType::atlas:
    case eCharacterType::demeter:
    case eCharacterType::dionysus:
    case eCharacterType::hades:
    case eCharacterType::hephaestus:
    case eCharacterType::hera:
    case eCharacterType::hermes:
    case eCharacterType::poseidon:
    case eCharacterType::zeus:
        return eLanguage::zeusText(64, 67);
    case eCharacterType::achilles:
    case eCharacterType::atalanta:
    case eCharacterType::bellerophon:
    case eCharacterType::hercules:
    case eCharacterType::jason:
    case eCharacterType::odysseus:
    case eCharacterType::perseus:
    case eCharacterType::theseus:
        return eLanguage::zeusText(64, 69);
    case eCharacterType::calydonianBoar:
    case eCharacterType::cerberus:
    case eCharacterType::chimera:
    case eCharacterType::cyclops:
    case eCharacterType::dragon:
    case eCharacterType::echidna:
    case eCharacterType::harpies:
    case eCharacterType::hector:
    case eCharacterType::hydra:
    case eCharacterType::kraken:
    case eCharacterType::maenads:
    case eCharacterType::medusa:
    case eCharacterType::minotaur:
    case eCharacterType::scylla:
    case eCharacterType::sphinx:
    case eCharacterType::talos:
    case eCharacterType::satyr:
        return eLanguage::zeusText(44, 163);

    case eCharacterType::settler:
        return eLanguage::zeusText(64, 1);

    case eCharacterType::homeless:
        return eLanguage::zeusText(64, 3);

    case eCharacterType::grower:
        return eLanguage::zeusText(64, 5);

    case eCharacterType::taxCollector:
        return eLanguage::zeusText(64, 7);
    case eCharacterType::fireFighter:
        return eLanguage::zeusText(64, 8);
    case eCharacterType::cartTransporter: {
        const auto ct = static_cast<const eCartTransporter*>(c);
        const auto type = ct->cartType();
        switch(type) {
        case eCartTransporterType::basic:
        case eCartTransporterType::ox:
            return eLanguage::zeusText(64, 9);
        case eCartTransporterType::food:
        case eCartTransporterType::arms:
        case eCartTransporterType::fleece:
        case eCartTransporterType::oil:
        case eCartTransporterType::wine:
        case eCartTransporterType::horse:
        case eCartTransporterType::chariot:
            return eLanguage::zeusText(64, 37);
        default:
            return eLanguage::zeusText(64, 9);
        }
    } break;

    case eCharacterType::rockThrower:
        return eLanguage::zeusText(64, 11);
    case eCharacterType::hoplite:
        return eLanguage::zeusText(64, 12);
    case eCharacterType::horseman:
        return eLanguage::zeusText(64, 13);

    case eCharacterType::competitor:
        return eLanguage::zeusText(64, 15);
    case eCharacterType::philosopher:
        return eLanguage::zeusText(64, 16);
    case eCharacterType::gymnast:
        return eLanguage::zeusText(64, 17);
    case eCharacterType::actor:
        return eLanguage::zeusText(64, 18);
    case eCharacterType::trader:
        return eLanguage::zeusText(64, 19) + " " + cname;
    case eCharacterType::tradeBoat:
        return eLanguage::zeusText(64, 20) + " " + cname;
    case eCharacterType::donkey:
        return eLanguage::zeusText(64, 21) + " " + cname;

    case eCharacterType::disgruntled:
        return eLanguage::zeusText(64, 23);

    case eCharacterType::fishingBoat:
        return eLanguage::zeusText(64, 25);
    case eCharacterType::peddler:
        return eLanguage::zeusText(64, 26);

    case eCharacterType::shepherd:
        return eLanguage::zeusText(64, 30);
    case eCharacterType::goatherd:
        return eLanguage::zeusText(64, 31);
    case eCharacterType::healer:
        return eLanguage::zeusText(64, 32);

    case eCharacterType::eliteCitizen:
        return eLanguage::zeusText(64, 38);
    case eCharacterType::archer:
        return eLanguage::zeusText(64, 39);

    case eCharacterType::trojanSpearthrower:
    case eCharacterType::centaurArcher:
    case eCharacterType::persianArcher:
    case eCharacterType::oceanidSpearthrower:
    case eCharacterType::egyptianArcher:
    case eCharacterType::atlanteanArcher:
    case eCharacterType::phoenicianArcher:
    case eCharacterType::mayanArcher:
    case eCharacterType::greekRockThrower:
        return eLanguage::zeusText(64, 40);
    case eCharacterType::trojanHoplite:
    case eCharacterType::persianHoplite:
    case eCharacterType::oceanidHoplite:
    case eCharacterType::egyptianHoplite:
    case eCharacterType::atlanteanHoplite:
    case eCharacterType::mayanHoplite:
    case eCharacterType::greekHoplite:
        return eLanguage::zeusText(64, 41);
    case eCharacterType::trojanHorseman:
    case eCharacterType::centaurHorseman:
    case eCharacterType::persianHorseman:
    case eCharacterType::egyptianChariot:
    case eCharacterType::atlanteanChariot:
    case eCharacterType::phoenicianHorseman:
    case eCharacterType::greekHorseman:
        return eLanguage::zeusText(64, 42);

    case eCharacterType::horse:
        return eLanguage::zeusText(64, 43);
    case eCharacterType::urchinGatherer:
        return eLanguage::zeusText(64, 44);

    case eCharacterType::sheep:
        return eLanguage::zeusText(64, 49);
    case eCharacterType::goat:
        return eLanguage::zeusText(64, 50);
    case eCharacterType::boar:
        return eLanguage::zeusText(64, 51);

    case eCharacterType::hunter:
        return eLanguage::zeusText(64, 53);

    case eCharacterType::lumberjack:
        return eLanguage::zeusText(64, 55);

    case eCharacterType::trireme:
        return eLanguage::zeusText(64, 57);
    case eCharacterType::artisan:
        return eLanguage::zeusText(64, 58);
    case eCharacterType::wolf:
        return eLanguage::zeusText(64, 59);
    case eCharacterType::waterDistributor:
        return eLanguage::zeusText(64, 60);
    case eCharacterType::watchman:
        return eLanguage::zeusText(64, 61);

    case eCharacterType::sick:
        return eLanguage::zeusText(64, 66);

    case eCharacterType::silverMiner:
    case eCharacterType::marbleMiner:
    case eCharacterType::bronzeMiner:
    case eCharacterType::orichalcMiner:
        return eLanguage::zeusText(64, 62);

    case eCharacterType::scholar:
        return eLanguage::zeusText(64, 72);
    case eCharacterType::astronomer:
        return eLanguage::zeusText(64, 73);
    case eCharacterType::inventor:
        return eLanguage::zeusText(64, 74);
    case eCharacterType::curator:
        return eLanguage::zeusText(64, 75);

    case eCharacterType::cattle1:
    case eCharacterType::cattle2:
    case eCharacterType::cattle3:
        return eLanguage::zeusText(64, 76);

    case eCharacterType::butcher:
        return eLanguage::zeusText(64, 77);

    case eCharacterType::ox:
        return eLanguage::zeusText(64, 79);
    default:
        return "";
    }
}

struct eCharMessage {
    std::string fText;
    eSoundVector* fSounds = nullptr;
    int fSoundId = -1;

    bool playSound() const {
        if(!fSounds) return false;
        fSounds->play(fSoundId, 1);
        return true;
    }
};

enum class eMinerState {
    goingTo,
    collecting,
    goingBack,
    other
};

eMinerState gGetMinerState(eCharacter * const c) {
    const auto a = c->actionType();
    switch(a) {
    case eCharacterActionType::walk:
        return eMinerState::goingTo;
    case eCharacterActionType::collect:
        return eMinerState::collecting;
    case eCharacterActionType::carry:
        return eMinerState::goingBack;
    default:
        return eMinerState::other;
    }
}

enum class eHunterState {
    goingTo,
    goingBack,
    other
};

eHunterState gGetHunterState(eCharacter * const c) {
    const auto a = c->actionType();
    switch(a) {
    case eCharacterActionType::walk:
        return eHunterState::goingTo;
    case eCharacterActionType::carry:
        return eHunterState::goingBack;
    default:
        return eHunterState::other;
    }
}

enum class eGrowerState {
    carryingOlives,
    carryingGrapes,
    carryingOranges,
    collectingOlives,
    collectingGrapes,
    collectingOranges,
    other
};

eGrowerState gGetGrowerState(eCharacter * const c) {
    const auto grow = static_cast<eGrower*>(c);
    const auto a = c->actionType();
    switch(a) {
    case eCharacterActionType::carry: {
        if(grow->olives()) {
            return eGrowerState::carryingOlives;
        } else if(grow->grapes()) {
            return eGrowerState::carryingGrapes;
        } else {
            return eGrowerState::carryingOranges;
        }
    } break;
    case eCharacterActionType::collect: {
        const auto tile = c->tile();
        const auto ub = tile->underBuildingType();
        switch(ub) {
        case eBuildingType::oliveTree:
            return eGrowerState::collectingOlives;
        case eBuildingType::vine:
            return eGrowerState::collectingGrapes;
        case eBuildingType::orangeTree:
            return eGrowerState::collectingOranges;
        default:
            return eGrowerState::other;
        }
    } break;
    default:
        return eGrowerState::other;
    }
}

eCharMessage gCharMessage(eCharacter * const c) {
    eCharMessage result;
    if(!c) return result;
    const auto cid = c->onCityId();
    auto& board = c->getBoard();
    const bool activeInv = board.hasActiveInvasions(cid);
    const auto tile = c->tile();
    bool enemyNearby = false;
    bool enemyImmortal = false;
    for(int k = 0; k < 10; k++) {
        eIterateSquare::iterateSquare(k, [&](const int dx, const int dy) {
            const auto t = tile->tileRel<eTile>(dx, dy);
            if(!t) return false;
            const auto& chars = t->characters();
            for(const auto& cc : chars) {
                const auto ct = c->teamId();
                const auto cct = cc->teamId();
                const bool e = eTeamIdHelpers::isEnemy(ct, cct);
                if(!e) continue;
                if(cc->isImmortal()) {
                    enemyImmortal = true;
                } else if(cc->isFighter()) {
                    enemyNearby = true;
                }
            }
            return false;
        });
    }
    const auto type = c->type();
    result.fSounds = eSounds::getCharacterVoices(c);
    const auto gMsgId = [&]() {
        const int health = board.health(cid);
        if(health < 70) return 0;
        const int pop = board.population(cid);
        const auto husbData = board.husbandryData(cid);
        if(husbData) {
            const int a = husbData->canSupport();
            if(a < pop) return 1;
        }
        const auto emplData = board.employmentData(cid);
        if(emplData) {
            const int u = emplData->unemployed();
            if(0.1*pop < u) return 2;
        }
        const bool vasal = !board.defeatedBy(cid).empty();
        if(vasal) return 3;
        // rebelling city 4
        const int popularity = board.popularity(cid);
        if(popularity < 80) return 5;
        return 6;
    };
    const int g = gMsgId();
    int groupId = -1;
    int stringId = -1;
    switch(type) {
    case eCharacterType::sheep:
    case eCharacterType::goat:
    case eCharacterType::wolf:
    case eCharacterType::deer:
    case eCharacterType::boar:
    case eCharacterType::cattle1:
    case eCharacterType::cattle2:
    case eCharacterType::cattle3:
        stringId = eRand::rand() % 4;
        break;
    case eCharacterType::settler: {
        const auto ed = board.employmentData(cid);
        const auto hd = board.husbandryData(cid);
        if(eRand::rand() % 2) {
            if(enemyImmortal) {
                stringId = 0;
            } else if(ed && ed->vacanciesFilledFraction() < 0.95) {
                stringId = 2;
            } else if(hd && hd->storedFood() > 6) {
                stringId = 3;
            } else {
                stringId = 1;
            }
        } else {
            stringId = 1;
        }
        groupId = 202;
    } break;
    case eCharacterType::homeless: {
        const auto limit = board.immigrationLimit(cid);
        if(enemyImmortal) {
            stringId = 0;
        } else if(limit == eImmigrationLimitedBy::unemployment) {
            stringId = 1;
        } else if(limit == eImmigrationLimitedBy::lackOfFood) {
            stringId = 2;
        } else if(limit == eImmigrationLimitedBy::highTaxes) {
            stringId = 3;
        } else if(limit == eImmigrationLimitedBy::lowWages) {
            stringId = 4;
        } else if(limit == eImmigrationLimitedBy::lackOfVacancies) {
            stringId = 5;
        } else if(limit == eImmigrationLimitedBy::excessiveMilitaryService) {
            stringId = 6;
        } else if(limit == eImmigrationLimitedBy::prolongedDebt) {
            stringId = 7;
        } else {
            stringId = 5;
        }
        groupId = 203;
    } break;
    case eCharacterType::disgruntled: {
        // groupId = 206;
        // groupId = 207;
        const auto limit = board.immigrationLimit(cid);
        if(enemyImmortal) {
            stringId = 0;
        } else if(limit == eImmigrationLimitedBy::unemployment) {
            stringId = 1;
        } else if(limit == eImmigrationLimitedBy::lackOfFood) {
            stringId = 2;
        } else if(limit == eImmigrationLimitedBy::highTaxes) {
            stringId = 3;
        } else if(limit == eImmigrationLimitedBy::lowWages) {
            stringId = 4;
        } else if(limit == eImmigrationLimitedBy::lackOfVacancies) {
            stringId = 1;
        } else if(limit == eImmigrationLimitedBy::excessiveMilitaryService) {
            stringId = 6;
        } else if(limit == eImmigrationLimitedBy::prolongedDebt) {
            stringId = 7;
        } else {
            stringId = 1;
        }
        groupId = 208;
        // groupId = 209;
    } break;
    case eCharacterType::peddler: {
        groupId = 229;
        const auto p = static_cast<ePeddler*>(c);
        const auto a = p->agora();
        if(enemyImmortal) {
            stringId = 0;
        } else if(!a || a->count(eResourceType::food) < 100) {
            stringId = 1 + eRand::rand() % 3;
        } else {
            stringId = 4 + eRand::rand() % 3;
        }
    } break;
    case eCharacterType::waterDistributor: {
        groupId = 238;
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
    } break;
    case eCharacterType::aphrodite:
    case eCharacterType::apollo:
    case eCharacterType::ares:
    case eCharacterType::artemis:
    case eCharacterType::athena:
    case eCharacterType::atlas:
    case eCharacterType::demeter:
    case eCharacterType::dionysus:
    case eCharacterType::hades:
    case eCharacterType::hephaestus:
    case eCharacterType::hera:
    case eCharacterType::hermes:
    case eCharacterType::poseidon:
    case eCharacterType::zeus: {
        switch(type) {
        case eCharacterType::aphrodite:
            groupId = 309;
            break;
        case eCharacterType::apollo:
            groupId = 306;
            break;
        case eCharacterType::ares:
            groupId = 307;
            break;
        case eCharacterType::artemis:
            groupId = 305;
            break;
        case eCharacterType::athena:
            groupId = 304;
            break;
        case eCharacterType::atlas:
            groupId = 354;
            break;
        case eCharacterType::demeter:
            groupId = 303;
            break;
        case eCharacterType::dionysus:
            groupId = 311;
            break;
        case eCharacterType::hades:
            groupId = 302;
            break;
        case eCharacterType::hephaestus:
            groupId = 308;
            break;
        case eCharacterType::hera:
            groupId = 353;
            break;
        case eCharacterType::hermes:
            groupId = 310;
            break;
        case eCharacterType::poseidon:
            groupId = 301;
            break;
        case eCharacterType::zeus:
            groupId = 300;
            break;
        default:
            break;
        }

        const auto g = static_cast<eGod*>(c);
        const auto a = g->attitude();
        switch(a) {
        case eGodAttitude::friendly:
            stringId = 0;
            break;
        case eGodAttitude::hostile:
            stringId = 1;
            break;
        case eGodAttitude::worshipped:
            bool provide = false;
            const auto a = g->action();
            if(a) {
                switch(a->type()) {
                case eCharActionType::aphroditeHelpAction:
                case eCharActionType::apolloHelpAction:
                case eCharActionType::aresHelpAction:
                case eCharActionType::artemisHelpAction:
                case eCharActionType::athenaHelpAction:
                case eCharActionType::atlasHelpAction:
                case eCharActionType::demeterHelpAction:
                case eCharActionType::dionysusHelpAction:
                case eCharActionType::hadesHelpAction:
                case eCharActionType::heraHelpAction:
                case eCharActionType::hephaestusHelpAction:
                case eCharActionType::hermesHelpAction:
                case eCharActionType::poseidonHelpAction:
                case eCharActionType::zeusHelpAction:
                    provide = true;
                    break;
                default:
                    provide = false;
                }
            }

            stringId = provide ? 2 : 0;
            break;
        }
    } break;
    case eCharacterType::achilles:
    case eCharacterType::atalanta:
    case eCharacterType::bellerophon:
    case eCharacterType::hercules:
    case eCharacterType::jason:
    case eCharacterType::odysseus:
    case eCharacterType::perseus:
    case eCharacterType::theseus: {
        stringId = g;
        switch(type) {
        case eCharacterType::achilles:
            groupId = 312;
            break;
        case eCharacterType::atalanta:
            groupId = 360;
            break;
        case eCharacterType::bellerophon:
            groupId = 359;
            break;
        case eCharacterType::hercules:
            groupId = 313;
            break;
        case eCharacterType::jason:
            groupId = 314;
            break;
        case eCharacterType::odysseus:
            groupId = 315;
            break;
        case eCharacterType::perseus:
            groupId = 316;
            break;
        case eCharacterType::theseus:
            groupId = 317;
            break;
        default:
            break;
        }
    } break;
    case eCharacterType::calydonianBoar:
        groupId = 319;
        stringId = 0;
        break;
    case eCharacterType::cerberus:
        groupId = 320;
        stringId = 0;
        break;
    case eCharacterType::chimera:
        groupId = 355;
        stringId = 0;
        break;
    case eCharacterType::cyclops:
        groupId = 321;
        stringId = 0;
        break;
    case eCharacterType::dragon:
        groupId = 322;
        stringId = 0;
        break;
    case eCharacterType::echidna:
        groupId = 356;
        stringId = 0;
        break;
    case eCharacterType::harpies:
        groupId = 357;
        stringId = 0;
        break;
    case eCharacterType::hector:
        groupId = 323;
        stringId = 0;
        break;
    case eCharacterType::hydra:
        groupId = 324;
        stringId = 0;
        break;
    case eCharacterType::kraken:
        groupId = 325;
        stringId = 0;
        break;
    case eCharacterType::maenads:
        groupId = 326;
        stringId = 0;
        break;
    case eCharacterType::medusa:
        groupId = 327;
        stringId = 0;
        break;
    case eCharacterType::minotaur:
        groupId = 328;
        stringId = 0;
        break;
    case eCharacterType::scylla:
        groupId = 329;
        stringId = 0;
        break;
    case eCharacterType::sphinx:
        groupId = 358;
        stringId = 0;
        break;
    case eCharacterType::talos:
        groupId = 330;
        stringId = 0;
        break;
    case eCharacterType::satyr:
        groupId = 318;
        stringId = 0;
        break;

    case eCharacterType::eliteCitizen:
        groupId = 210;
        if(enemyImmortal) {
            stringId = 7;
        } else if(c->fighting()) {
            stringId = 8;
        }
        break;
    case eCharacterType::marbleMiner: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 0;
        } else if(eRand::rand() % 2) {
            const auto state = gGetMinerState(c);
            switch(state) {
            case eMinerState::goingTo:
                stringId = 8;
                break;
            case eMinerState::goingBack:
                stringId = 9;
                break;
            case eMinerState::collecting:
                stringId = 10;
                break;
            default:
                break;
            }
        }
        groupId = 220;
    } break;
    case eCharacterType::bronzeMiner: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            const auto state = gGetMinerState(c);
            switch(state) {
            case eMinerState::goingTo:
                stringId = 8;
                break;
            case eMinerState::goingBack:
            case eMinerState::collecting:
                stringId = 9;
                break;
            default:
                break;
            }
        }
        groupId = 218;
    } break;
    case eCharacterType::silverMiner: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            const auto state = gGetMinerState(c);
            switch(state) {
            case eMinerState::goingTo:
                stringId = 8;
                break;
            case eMinerState::goingBack:
            case eMinerState::collecting:
                stringId = 9;
                break;
            default:
                break;
            }
        }
        groupId = 219;
    } break;
    case eCharacterType::actor: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        }
        groupId = 233;
    } break;
    case eCharacterType::fireFighter: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2 || c->fighting()) {
            const int m = board.maintanance(cid);
            if(c->fighting()) {
                stringId = 8 + eRand::rand() % 3;
            } else if(m < 75) {
                stringId = 11;
            } else {
                stringId = 12;
            }
        }
        groupId = 240;
    } break;
    case eCharacterType::sick: {
        if(enemyImmortal) {
            stringId = 0;
        } else {
            stringId = 1;
        }
        groupId = 205;
    } break;
    case eCharacterType::philosopher: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 236;
    } break;
    case eCharacterType::lumberjack: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            const auto a = c->actionType();
            if(a == eCharacterActionType::carry) {
                stringId = 8 + eRand::rand() % 2;
            } else if(a == eCharacterActionType::collect) {
                stringId = 8;
            }
        }
        groupId = 217;
    } break;
    case eCharacterType::artisan: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 221;
    } break;
    case eCharacterType::astronomer: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 346;
    } break;
    case eCharacterType::inventor: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 347;
    } break;
    case eCharacterType::curator: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 348;
    } break;
    case eCharacterType::scholar: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 345;
    } break;
    case eCharacterType::competitor: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 235;
    } break;
    case eCharacterType::gymnast: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 234;
    } break;
    case eCharacterType::taxCollector: {
        if(enemyImmortal) {
            stringId = 7;
        } else {
            stringId = g;
        }
        groupId = 242;
    } break;
    case eCharacterType::watchman: {
        if(enemyImmortal) {
            stringId = 0;
        } else if(c->fighting()) {
            stringId = 4 + eRand::rand() % 2;
        } else {
            const int unrest = board.unrest(cid);
            if(unrest == 0) {
                stringId = 1;
            } else if(unrest < 5) {
                stringId = 2;
            } else {
                stringId = 3;
            }
        }
        groupId = 241;
    } break;
    case eCharacterType::fishingBoat: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            stringId = 8 + eRand::rand() % 4;
        }
        groupId = 214;
    } break;
    case eCharacterType::grower: {
        const auto gro = static_cast<eGrower*>(c);
        stringId = g;
        const auto type = gro->growerType();
        const auto state = gGetGrowerState(c);
        switch(type) {
        case eGrowerType::grapesAndOlives: {
            groupId = 216;
            switch(state) {
            case eGrowerState::collectingOlives:
                stringId = 8;
                break;
            case eGrowerState::carryingOlives:
                stringId = 9;
                break;
            case eGrowerState::collectingGrapes:
                stringId = 10;
                break;
            case eGrowerState::carryingGrapes:
                stringId = 11;
                break;
            default:
                break;
            }
        } break;
        case eGrowerType::oranges: {
            groupId = 340;
            switch(state) {
            case eGrowerState::collectingOranges:
                stringId = 8;
                break;
            case eGrowerState::carryingOranges:
                stringId = 9;
                break;
            default:
                break;
            }
        } break;
        }
        if(enemyImmortal) {
            stringId = 7;
        }
    } break;
    case eCharacterType::shepherd: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            stringId = 8 + eRand::rand() % 4;
        }
        groupId = 212;
    } break;
    case eCharacterType::goatherd: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            stringId = 8 + eRand::rand() % 4;
        }
        groupId = 213;
    } break;
    case eCharacterType::urchinGatherer: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            stringId = 8 + eRand::rand() % 4;
        }
        groupId = 215;
    } break;
    case eCharacterType::butcher: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            stringId = 8 + eRand::rand() % 3;
        }
        groupId = 342;
    } break;
    case eCharacterType::tradeBoat: {
        if(enemyImmortal) {
            stringId = 0;
        } else {
            stringId = 6 + eRand::rand() % 2;
        }
        groupId = 231;
    } break;
    case eCharacterType::trader: {
        if(enemyImmortal) {
            stringId = 0;
        } else {
            stringId = 6 + eRand::rand() % 2;
        }
        groupId = 232;
    } break;
    case eCharacterType::trireme: {
        if(enemyImmortal) {
            stringId = 0;
        } else if(enemyNearby) {
            stringId = 3 + eRand::rand() % 2;
        } else if(activeInv) {
            stringId = 2;
        } else {
            stringId = 1;
        }
        groupId = 247;
    } break;
    case eCharacterType::healer: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            const int health = board.health(cid);
            if(health < 70) {
                stringId = 8;
            }
        }
        groupId = 239;
    } break;
    case eCharacterType::hunter: {
        stringId = g;
        if(enemyImmortal) {
            stringId = 7;
        } else if(eRand::rand() % 2) {
            const auto state = gGetHunterState(c);
            switch(state) {
            case eHunterState::goingTo:
                stringId = 8;
                break;
            case eHunterState::goingBack:
                stringId = 9;
                break;
            default:
                break;
            }
        }
        const auto h = static_cast<eHunter*>(c);
        const bool deer = h->deerHunter();
        if(deer) {
            groupId = 341;
        } else {
            groupId = 211;
        }
    } break;
    case eCharacterType::cartTransporter: {
        const auto ct = static_cast<eCartTransporter*>(c);
        const auto type = ct->cartType();
        const int count = ct->resCount();
        const bool empty = count == 0;
        switch(type) {
        case eCartTransporterType::food:
        case eCartTransporterType::fleece:
        case eCartTransporterType::oil:
        case eCartTransporterType::wine:
        case eCartTransporterType::arms:
        case eCartTransporterType::horse:
        case eCartTransporterType::chariot: {
            if(empty) {
                stringId = 1;
            } else {
                stringId = 2;
            }
        } break;
        case eCartTransporterType::basic: {
            const auto a = c->actionType();
            if(empty) {

            } else {
                if(a == eCharacterActionType::stand) {
                    stringId = 2;
                } else {
                    stringId = 3;
                }
            }
        } break;
        default:
            break;
        }
        switch(type) {
        case eCartTransporterType::food:
            groupId = 222;
            break;
        case eCartTransporterType::fleece:
            groupId = 223;
            break;
        case eCartTransporterType::oil:
            groupId = 224;
            break;
        case eCartTransporterType::wine:
            groupId = 225;
            break;
        case eCartTransporterType::arms:
            groupId = 226;
            break;
        case eCartTransporterType::horse:
            groupId = 227;
            break;
        case eCartTransporterType::chariot:
            groupId = 344;
            break;
        case eCartTransporterType::basic:
            groupId = 230;
            break;
        default:
            break;
        }
        if(enemyImmortal) {
            stringId = 0;
        }
    } break;
    case eCharacterType::chariotPoseidon: {
        if(enemyImmortal) {
            stringId = 0;
        } else if(enemyNearby) {
            stringId = 3 + eRand::rand() % 2;
        } else if(activeInv) {
            stringId = 2;
        } else {
            stringId = 1;
        }
        groupId = 351;
    } break;
    case eCharacterType::hoplite:
    case eCharacterType::hoplitePoseidon: {
        if(enemyImmortal) {
            stringId = 0;
        } else if(enemyNearby) {
            stringId = 3 + eRand::rand() % 2;
        } else if(activeInv) {
            stringId = 2;
        } else {
            stringId = 1;
        }
        groupId = 245;
    } break;
    case eCharacterType::horseman: {
        if(enemyImmortal) {
            stringId = 0;
        } else if(enemyNearby) {
            stringId = 3 + eRand::rand() % 2;
        } else if(activeInv) {
            stringId = 2;
        } else {
            stringId = 1;
        }
        groupId = 244;
    } break;
    case eCharacterType::rockThrower: {
        if(enemyImmortal) {
            stringId = 0;
        } else if(enemyNearby) {
            stringId = 3 + eRand::rand() % 2;
        } else if(activeInv) {
            stringId = 2;
        } else {
            stringId = 1;
        }
        groupId = 246;
    } break;
    case eCharacterType::amazon: {
        if(enemyImmortal) {
            stringId = 0;
        } else if(enemyNearby) {
            stringId = 3 + eRand::rand() % 2;
        } else if(activeInv) {
            stringId = 2;
        } else {
            stringId = 1;
        }
        groupId = 349;
    } break;
    }

    result.fText = eLanguage::zeusText(groupId, stringId);
    result.fSoundId = stringId;

    return result;
}

void eCharacterInfoWidget::initialize(const std::vector<eCharacter*> chars) {
    eInfoWidget::initialize("");
    if(chars.empty()) return;

    const auto res = resolution();
    if(chars.size() > 1) {
        const auto w = new eWidget(window());

        for(eCharacter* const c : chars) {
            const auto portrait = gCharPortrait(res, c);
            const auto b = new eButtonBase(window());
            b->setTexture(portrait);
            b->setPressAction([this, c]() {
                setCharacter(c);
            });
            b->fitContent();
            w->addWidget(b);
        }

        w->stackHorizontally();
        w->fitContent();
        addInfoWidget(w);
    }

    eCharacter * const c = chars[0];
    const auto type = c->type();
    const auto portrait = gCharPortrait(res, c);

    mContent = new eWidget(window());
    mContent->setWidth(widgetWidth());

    mPortraitLabel = new eLabel(window());
    mPortraitLabel->setTexture(portrait);
    mPortraitLabel->fitContent();

    const int p = res.smallPadding();
    const int remW = widgetWidth() - mPortraitLabel->width() - 2*p;

    mTextWidget = new eWidget(window());
    mTextWidget->setWidth(remW);

    const int seedId = c->seedId();
    const auto name = gCharName(type, seedId);

    mNameLabel = new eLabel(name, window());
    mNameLabel->setWrapWidth(remW);
    mNameLabel->setHugeFontSize();
    mNameLabel->setTinyPadding();
    mNameLabel->fitContent();
    mTextWidget->addWidget(mNameLabel);

    const auto occupation = gCharOccupation(c);
    mOccupationLabel = new eLabel(occupation, window());
    mOccupationLabel->setWrapWidth(remW);
    mOccupationLabel->setTinyPadding();
    mOccupationLabel->fitContent();
    mTextWidget->addWidget(mOccupationLabel);

    const auto msg = gCharMessage(c);
    mMsgLabel = new eLabel(window());
    mMsgLabel->setTinyPadding();
    mMsgLabel->setSmallFontSize();
    mMsgLabel->setWrapWidth(remW);
    mMsgLabel->setText(msg.fText);
    mMsgLabel->fitContent();
    mTextWidget->addWidget(mMsgLabel);

    const auto speakButton = new eBasicButton(
        &eInterfaceTextures::fSpeakButton, window());
    speakButton->setPressAction([this]() {
        if(mSpeakAction) mSpeakAction();
    });
    mTextWidget->addWidget(speakButton);
    speakButton->align(eAlignment::right);

    mTextWidget->stackVertically(p);

    mContent->addWidget(mPortraitLabel);
    mContent->addWidget(mTextWidget);
    mContent->stackHorizontally(p);
    mContent->fitContent();
    addInfoWidget(mContent);

    setCharacter(c);
}

void eCharacterInfoWidget::setCharacter(eCharacter * const c) {
    const auto type = c->type();
    const auto res = resolution();
    const auto portrait = gCharPortrait(res, c);

    mPortraitLabel->setTexture(portrait);
    mPortraitLabel->fitContent();

    const int seedId = c->seedId();
    const auto name = gCharName(type, seedId);
    mNameLabel->setText(name);
    mNameLabel->fitContent();

    const auto occupation = gCharOccupation(c);
    mOccupationLabel->setText(occupation);
    mOccupationLabel->fitContent();

    const auto msg = gCharMessage(c);
    mMsgLabel->setText(msg.fText);
    msg.playSound();
    mSpeakAction = [msg]() {
        msg.playSound();
    };
    mMsgLabel->fitContent();

    const int p = res.smallPadding();
    mTextWidget->stackVertically(p);
    mTextWidget->fitHeight();
    mContent->fitHeight();
}
