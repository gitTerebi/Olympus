#include "egiftsizedialog.h"

#include "elanguage.h"
#include "estringhelpers.h"
#include "elabel.h"
#include "eframedbuttonwithicon.h"
#include "engine/egameboard.h"
#include "engine/egifthelpers.h"

void eGiftSizeDialog::initialize(const eResourceType type,
                                 const stdsptr<eWorldCity>& c,
                                 const eRequestFunction& func,
                                 const eGameBoard& board) {
    const auto r = resolution();
    const double mult = r.multiplier();

    setType(eFrameType::message);

    const auto innerWid = new eWidget(window());
    addWidget(innerWid);
    const int p = std::round(20*mult);
    innerWid->setNoPadding();
    innerWid->move(p, p/2);

    const auto name = c->name();
    auto rof = eLanguage::zeusText(41, 0); // give to
    eStringHelpers::replace(rof, "[city_name]", name);
    const auto rofLabel = new eLabel(window());
    rofLabel->setTinyFontSize();
    rofLabel->setSmallPadding();
    rofLabel->setText(rof);
    rofLabel->fitContent();

    innerWid->addWidget(rofLabel);

    const int baseCount = eGiftHelpers::giftCount(type);
    int avCount = 0;
    const auto cids = board.personPlayerCities();
    for(const auto cid : cids) {
        const int r = board.resourceCount(cid, type);
        if(r > avCount) avCount = r;
    }
    using gsType = std::vector<std::pair<int, int>>;
    gsType gs{{baseCount, 22}, // small
              {2*baseCount, 23}, // medium
              {3*baseCount, 24}}; // large
    const auto typeStr = eResourceTypeHelpers::typeLongName(type);
    for(const auto& g : gs) {
        const int count = g.first;
        if(count > avCount) break;
        const auto countStr = std::to_string(count);
        const int string = g.second;
        auto giftStr = eLanguage::zeusText(41, string);
        eStringHelpers::replace(giftStr, "[amount]", countStr);
        eStringHelpers::replace(giftStr, "[item]", typeStr);

//        const auto boardPtr = &board;
        const auto b = new eFramedButtonWithIcon(window());
        b->setPressAction([/*boardPtr, */type, count, func]() {
//            const auto& board = *boardPtr;
//            const auto cids = board.personPlayerCities();
//            std::vector<eCityId> validCids;
//            for(const auto cid : cids) {
//                const int r = board.resourceCount(cid, type);
//                if(r >= count) {
//                    validCids.push_back(cid);
//                }
//            }
//            if(validCids.empty()) return;

            func(type, count);
        });
        b->initialize(type, giftStr);
        innerWid->addWidget(b);
    }

    innerWid->stackVertically();
    innerWid->fitContent();

    fitContent();
    rofLabel->align(eAlignment::hcenter);
}
