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
#include "widgets/ebuttonbase.h"
#include "engine/eworldcity.h"
#include "engine/egameboard.h"

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

    default:
        return zgp.getTexture(0);
    }
}

std::string gCharName(const eCharacterType c, const int seedId) {
    switch(c) {
    case eCharacterType::settler:
    case eCharacterType::homeless:
    case eCharacterType::sick:
    case eCharacterType::disgruntled:
    case eCharacterType::actor:
    case eCharacterType::philosopher:
    case eCharacterType::competitor:
    case eCharacterType::gymnast:
    case eCharacterType::scholar:
    case eCharacterType::astronomer:
    case eCharacterType::cartTransporter:
    case eCharacterType::inventor:
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
            return eLanguage::zeusText(64, 9);
        case eCartTransporterType::food:
        case eCartTransporterType::arms:
        case eCartTransporterType::fleece:
        case eCartTransporterType::oil:
        case eCartTransporterType::wine:
        case eCartTransporterType::horse:
        case eCartTransporterType::ox:
        case eCartTransporterType::chariot:
            return eLanguage::zeusText(64, 4);
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

    case eCharacterType::ox:
        return eLanguage::zeusText(64, 79);

    case eCharacterType::butcher:
        return eLanguage::zeusText(64, 77);

    case eCharacterType::scholar:
        return eLanguage::zeusText(64, 72);
    case eCharacterType::astronomer:
        return eLanguage::zeusText(64, 73);

    case eCharacterType::cattle1:
    case eCharacterType::cattle2:
    case eCharacterType::cattle3:
        return eLanguage::zeusText(64, 76);
    default:
        return "";
    }
}

std::string gCharMessage(eCharacter * const c) {
    if(!c) return "";
    return "";
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

    const auto w = new eWidget(window());
    w->setWidth(widgetWidth());

    mPortraitLabel = new eLabel(window());
    mPortraitLabel->setTexture(portrait);
    mPortraitLabel->fitContent();

    const int p = res.smallPadding();
    const int remW = widgetWidth() - mPortraitLabel->width() - 2*p;

    mTextWidget = new eWidget(window());

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
    mMsgLabel->setText(msg);
    mMsgLabel->fitContent();
    mTextWidget->addWidget(mMsgLabel);

    mTextWidget->stackVertically(p);
    w->addWidget(mPortraitLabel);
    w->addWidget(mTextWidget);
    w->stackHorizontally(p);
    w->fitContent();
    addInfoWidget(w);

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
    mMsgLabel->setWrapWidth(widgetWidth());
    mMsgLabel->setText(msg);
    mMsgLabel->fitContent();

    const int p = res.smallPadding();
    mTextWidget->stackVertically(p);
}
