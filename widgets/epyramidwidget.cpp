#include "epyramidwidget.h"

#include "buildings/pyramids/epyramid.h"
#include "eswitchbutton.h"
#include "egodbutton.h"

void ePyramidWidget::initialize(const ePSptr& e, const eAction& updater) {
    setType(eFrameType::message);

    const auto cont = new eWidget(window());
    cont->setNoPadding();
    addWidget(cont);
    const int p = padding();

    bool chooseGod = false;
    switch(e->fType) {
    case eBuildingType::minorShrineAphrodite:
    case eBuildingType::minorShrineApollo:
    case eBuildingType::minorShrineAres:
    case eBuildingType::minorShrineArtemis:
    case eBuildingType::minorShrineAthena:
    case eBuildingType::minorShrineAtlas:
    case eBuildingType::minorShrineDemeter:
    case eBuildingType::minorShrineDionysus:
    case eBuildingType::minorShrineHades:
    case eBuildingType::minorShrineHephaestus:
    case eBuildingType::minorShrineHera:
    case eBuildingType::minorShrineHermes:
    case eBuildingType::minorShrinePoseidon:
    case eBuildingType::minorShrineZeus: // 3x3

    case eBuildingType::shrineAphrodite:
    case eBuildingType::shrineApollo:
    case eBuildingType::shrineAres:
    case eBuildingType::shrineArtemis:
    case eBuildingType::shrineAthena:
    case eBuildingType::shrineAtlas:
    case eBuildingType::shrineDemeter:
    case eBuildingType::shrineDionysus:
    case eBuildingType::shrineHades:
    case eBuildingType::shrineHephaestus:
    case eBuildingType::shrineHera:
    case eBuildingType::shrineHermes:
    case eBuildingType::shrinePoseidon:
    case eBuildingType::shrineZeus: // 6x6

    case eBuildingType::majorShrineAphrodite:
    case eBuildingType::majorShrineApollo:
    case eBuildingType::majorShrineAres:
    case eBuildingType::majorShrineArtemis:
    case eBuildingType::majorShrineAthena:
    case eBuildingType::majorShrineAtlas:
    case eBuildingType::majorShrineDemeter:
    case eBuildingType::majorShrineDionysus:
    case eBuildingType::majorShrineHades:
    case eBuildingType::majorShrineHephaestus:
    case eBuildingType::majorShrineHera:
    case eBuildingType::majorShrineHermes:
    case eBuildingType::majorShrinePoseidon:
    case eBuildingType::majorShrineZeus: // 8x8
        chooseGod = true;
    default:
        break;
    }

    if(chooseGod) {
        const auto b = new eGodButton(window());
        b->initialize([e, updater](const eGodType god) {
            const int godId = static_cast<int>(god);
            eBuildingType type;
            switch(e->fType) {
            case eBuildingType::minorShrineAphrodite:
            case eBuildingType::minorShrineApollo:
            case eBuildingType::minorShrineAres:
            case eBuildingType::minorShrineArtemis:
            case eBuildingType::minorShrineAthena:
            case eBuildingType::minorShrineAtlas:
            case eBuildingType::minorShrineDemeter:
            case eBuildingType::minorShrineDionysus:
            case eBuildingType::minorShrineHades:
            case eBuildingType::minorShrineHephaestus:
            case eBuildingType::minorShrineHera:
            case eBuildingType::minorShrineHermes:
            case eBuildingType::minorShrinePoseidon:
            case eBuildingType::minorShrineZeus: {
                type = eBuildingType::minorShrineAphrodite;
            } break;

            case eBuildingType::shrineAphrodite:
            case eBuildingType::shrineApollo:
            case eBuildingType::shrineAres:
            case eBuildingType::shrineArtemis:
            case eBuildingType::shrineAthena:
            case eBuildingType::shrineAtlas:
            case eBuildingType::shrineDemeter:
            case eBuildingType::shrineDionysus:
            case eBuildingType::shrineHades:
            case eBuildingType::shrineHephaestus:
            case eBuildingType::shrineHera:
            case eBuildingType::shrineHermes:
            case eBuildingType::shrinePoseidon:
            case eBuildingType::shrineZeus: {
                type = eBuildingType::shrineAphrodite;
            } break;

            case eBuildingType::majorShrineAphrodite:
            case eBuildingType::majorShrineApollo:
            case eBuildingType::majorShrineAres:
            case eBuildingType::majorShrineArtemis:
            case eBuildingType::majorShrineAthena:
            case eBuildingType::majorShrineAtlas:
            case eBuildingType::majorShrineDemeter:
            case eBuildingType::majorShrineDionysus:
            case eBuildingType::majorShrineHades:
            case eBuildingType::majorShrineHephaestus:
            case eBuildingType::majorShrineHera:
            case eBuildingType::majorShrineHermes:
            case eBuildingType::majorShrinePoseidon:
            case eBuildingType::majorShrineZeus: {
                type = eBuildingType::majorShrineAphrodite;
            } break;
            default:
                assert(false);
                break;
            }

            const int typeId = static_cast<int>(type) + godId;
            e->fType = static_cast<eBuildingType>(typeId);

            updater();
        });
        b->setType(ePyramid::sGod(e->fType));
        cont->addWidget(b);
    }

    const int iMax = e->fLevels.size();
    for(int i = 0; i < iMax; i++) {
        const bool l = e->fLevels[i];
        const auto b = new eSwitchButton(window());
        const auto iStr = std::to_string(i);
        b->addValue("Level " + iStr + " Black");
        b->addValue("Level " + iStr + " White");
        b->fitValidContent();
        b->setValue(l ? 0 : 1);
        cont->addWidget(b);
        b->setSwitchAction([e, i, updater](const int) {
            e->fLevels[i] = !e->fLevels[i];
            updater();
        });
    }

    cont->stackVertically(p);
    cont->fitContent();
    const auto& cs = cont->children();
    for(const auto c : cs) {
        c->align(eAlignment::hcenter);
    }

    cont->align(eAlignment::center);
}
