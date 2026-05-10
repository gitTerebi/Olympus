#include "egamewidget.h"
#include "infowidgets/einfowidget.h"

#include "elanguage.h"

#include "buildings/allbuildings.h"
#include "buildings/efarmbase.h"
#include "buildings/eshepherbuildingbase.h"
#include "enumbers.h"

#include "engine/edate.h"

#include "evectorhelpers.h"
#include "infowidgets/eagorainfowidget.h"
#include "infowidgets/echaracterinfowidget.h"
#include "infowidgets/ecommonhouseinfowidget.h"
#include "infowidgets/eheroshallinfowidget.h"
#include "infowidgets/ehippodromeinfowidget.h"
#include "infowidgets/esanctuaryinfowidget.h"
#include "infowidgets/estorageinfowidget.h"
#include "infowidgets/etradepostinfowidget.h"
#include "infowidgets/etriremewharfinfowidget.h"

#include "engine/e-game-board.h"

eInfoWidget* eGameWidget::openInfoWidget(eBuilding* const b) {
    eInfoWidget* wid = nullptr;
    eAction closeAct;
    if(const auto house = dynamic_cast<eHouseBase*>(b)) {
        const auto w = new eCommonHouseInfoWidget(window(), this);
        w->initialize(house);
        wid = w;
    } else if(const auto stor = dynamic_cast<eTradePost*>(b)) {
        const auto storWid = new eTradePostInfoWidget(window(), this);
        const auto prevNext = [this, storWid, stor, closeAct](const bool next) {
            const auto pid = stor->playerId();
            const auto& board = stor->getBoard();
            std::vector<eTradePost*> posts;
            const auto cities = board.playerCitiesOnBoard(pid);
            for(const auto cid : cities) {
                const auto cboard = board.boardCityWithId(cid);
                const auto& cposts = cboard->tradePosts();
                for(const auto p : cposts) {
                    posts.emplace_back(p);
                }
            }
            if(posts.size() <= 1) return;
            int id = eVectorHelpers::index(posts, stor);
            if(next) {
                id++;
            } else {
                id--;
            }
            if(id < 0) id = posts.size() - 1;
            else if(id >= int(posts.size())) id = 0;
            storWid->close();
            const auto post = posts[id];
            openInfoWidget(post);
            const auto tile = post->centerTile();
            viewTile(tile);
        };
        storWid->initialize(stor, prevNext);
        wid = storWid;
    } else if(const auto stor = dynamic_cast<eStorageBuilding*>(b)) {
        const auto storWid = new eStorageInfoWidget(window(), this);
        storWid->initialize(stor);
        wid = storWid;
    } else if(const auto hp = dynamic_cast<eHippodromePiece*>(b)) {
        const auto h = hp->hippodrome();
        if(h) {
            const auto hWid = new eHippodromeInfoWidget(window(), this, true, true);
            hWid->initialize(h);
            wid = hWid;
        }
    } else if(const auto hh = dynamic_cast<eHerosHall*>(b)) {
        const auto hhWid = new eHerosHallInfoWidget(window(), this, false, false);
        hhWid->initialize(hh);
        wid = hhWid;
    } else if(const auto tw = dynamic_cast<eTriremeWharf*>(b)) {
        const auto twWid = new eTriremeWharfInfoWidget(window(), this, false, false);
        twWid->initialize(tw);
        wid = twWid;
    } else if(const auto encl = dynamic_cast<eHorseRanchEnclosure*>(b)) {
        return openInfoWidget(encl->ranch());
    } else {
        eAgoraBase* a = nullptr;
        if(const auto aa = dynamic_cast<eAgoraBase*>(b)) {
            a = aa;
        } else if(const auto v = dynamic_cast<eVendor*>(b)) {
            a = v->agora();
        } else if(const auto v = dynamic_cast<eAgoraSpace*>(b)) {
            a = v->agora();
        } else if(const auto v = dynamic_cast<eRoad*>(b)) {
            a = v->underAgora();
        }
        if(a) {
            const auto aWid = new eAgoraInfoWidget(window(), this);
            aWid->initialize(a);
            wid = aWid;
        } else if(const auto s = dynamic_cast<eMonument*>(b)) {
            const auto sWid = new eSanctuaryInfoWidget(window(), this);
            sWid->initialize(s);
            wid = sWid;
        } else if(const auto sb = dynamic_cast<eSanctBuilding*>(b)) {
            return openInfoWidget(sb->monument());
        } else if(const auto fb = dynamic_cast<eFarmBase*>(b)) {
            const auto ebWid = new eInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(fb, title, info,
                                 employmentInfo,
                                 additionalInfo);
            ebWid->initialize(title);
            const int p = fb->productionPercent();
            int group = 112; // default wheat
            const auto rt = fb->resourceType();
            switch(rt) {
            case eResourceType::wheat:
                group = 112;
                break;
            case eResourceType::onions:
                group = 113;
                break;
            case eResourceType::carrots:
                group = 114;
                break;
            default:
                group = 112;
                break;
            }
            std::string prodStr = eLanguage::zeusText(group, 2) + " " + std::to_string(p) + "% " + eLanguage::zeusText(group, 3);
            ebWid->addText(prodStr);
            const int maxYearly = (360 * eNumbers::sDayLength) / eNumbers::sFarmRipePeriod * 4;
            const double eff = maxYearly > 0 ? (double)fb->producedThisYear() / maxYearly : 0;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(fb->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            // add employment widget
            const int pp = ebWid->padding();
            const auto empWid = ebWid->addFramedWidget(8*pp);
            const int e = fb->employed();
            const int me = fb->maxEmployees();
            const auto estr = std::to_string(e);
            const auto mestr = std::to_string(me);
            const auto emplstr = eLanguage::zeusText(8, 13);
            const auto memplstr = eLanguage::zeusText(69, 0); // needed )
            const auto str = estr + " " + emplstr + "  (" + mestr + " " + memplstr;
            const auto empl = new eLabel(str, window());
            empl->setFontSizeS();
            empl->setPaddingS();
            empl->fitContent();
            empWid->addWidget(empl);
            empl->align(eAlignment::hcenter);
            const eMonth hm = fb->nextHarvestMonth();
            std::string harvestStr = eLanguage::zeusText(group, 14) + " " + eMonthHelper::name(hm) + ".";
            ebWid->addText(harvestStr);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto pb = dynamic_cast<eProcessingBuilding*>(b)) {
            const auto ebWid = new eInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(pb, title, info,
                                 employmentInfo,
                                 additionalInfo);
            ebWid->initialize(title);
            const int p = pb->productionPercent();
            int group = 122; // default winery
            const auto type = pb->type();
            switch(type) {
            case eBuildingType::winery:
                group = 122;
                break;
            case eBuildingType::armory:
                group = 124;
                break;
            case eBuildingType::olivePress:
                group = 125;
                break;
            case eBuildingType::sculptureStudio:
                group = 126;
                break;
            case eBuildingType::corral:
                group = 141;
                break;
            case eBuildingType::huntingLodge:
                group = 154;
                break;

            case eBuildingType::chariotFactory:
                group = 281;
                break;
            default:
                group = 122;
                break;
            }
            std::string prodStr = eLanguage::zeusText(group, 2) + " " + std::to_string(p) + "% " + eLanguage::zeusText(group, 3);
            ebWid->addText(prodStr);
            const int maxYearly = (360 * eNumbers::sDayLength) / pb->processWaitTime();
            const double eff = maxYearly > 0 ? (double)pb->producedThisYear() / maxYearly : 0;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(pb->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            // add employment widget
            const int pp = ebWid->padding();
            const auto empWid = ebWid->addFramedWidget(8*pp);
            const int e = pb->employed();
            const int me = pb->maxEmployees();
            const auto estr = std::to_string(e);
            const auto mestr = std::to_string(me);
            const auto emplstr = eLanguage::zeusText(8, 13);
            const auto memplstr = eLanguage::zeusText(69, 0);
            const auto str = estr + " " + emplstr + "  (" + mestr + " " + memplstr;
            const auto empl = new eLabel(str, window());
            empl->setFontSizeS();
            empl->setPaddingS();
            empl->fitContent();
            empWid->addWidget(empl);
            empl->align(eAlignment::hcenter);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto sb = dynamic_cast<eShepherBuildingBase*>(b)) {
            const auto ebWid = new eInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(sb, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            constexpr int maxYearly = 8;
            const double eff = (double)sb->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(sb->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            const int pp = ebWid->padding();
            const auto empWid = ebWid->addFramedWidget(8*pp);
            const int e = sb->employed();
            const int me = sb->maxEmployees();
            const auto emplstr = eLanguage::zeusText(8, 13);
            const auto memplstr = eLanguage::zeusText(69, 0);
            const auto str = std::to_string(e) + " " + emplstr + "  (" + std::to_string(me) + " " + memplstr;
            const auto empl = new eLabel(str, window());
            empl->setFontSizeS();
            empl->setPaddingS();
            empl->fitContent();
            empWid->addWidget(empl);
            empl->align(eAlignment::hcenter);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto gl = dynamic_cast<eGrowersLodge*>(b)) {
            const auto ebWid = new eInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(gl, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            constexpr int maxYearly = 20;
            const double eff = (double)gl->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(gl->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            const int pp = ebWid->padding();
            const auto empWid = ebWid->addFramedWidget(8*pp);
            const int e = gl->employed();
            const int me = gl->maxEmployees();
            const auto emplstr = eLanguage::zeusText(8, 13);
            const auto memplstr = eLanguage::zeusText(69, 0);
            const auto str = std::to_string(e) + " " + emplstr + "  (" + std::to_string(me) + " " + memplstr;
            const auto empl = new eLabel(str, window());
            empl->setFontSizeS();
            empl->setPaddingS();
            empl->fitContent();
            empWid->addWidget(empl);
            empl->align(eAlignment::hcenter);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto hl = dynamic_cast<eHuntingLodge*>(b)) {
            const auto ebWid = new eInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(hl, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            constexpr int maxYearly = 12;
            const double eff = (double)hl->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(hl->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            const int pp = ebWid->padding();
            const auto empWid = ebWid->addFramedWidget(8*pp);
            const int e = hl->employed();
            const int me = hl->maxEmployees();
            const auto emplstr = eLanguage::zeusText(8, 13);
            const auto memplstr = eLanguage::zeusText(69, 0);
            const auto str = std::to_string(e) + " " + emplstr + "  (" + std::to_string(me) + " " + memplstr;
            const auto empl = new eLabel(str, window());
            empl->setFontSizeS();
            empl->setPaddingS();
            empl->fitContent();
            empWid->addWidget(empl);
            empl->align(eAlignment::hcenter);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto fy = dynamic_cast<eFishery*>(b)) {
            const auto ebWid = new eInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(fy, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            constexpr int maxYearly = 13;
            const double eff = (double)fy->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(fy->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            const int pp = ebWid->padding();
            const auto empWid = ebWid->addFramedWidget(8*pp);
            const int e = fy->employed();
            const int me = fy->maxEmployees();
            const auto emplstr = eLanguage::zeusText(8, 13);
            const auto memplstr = eLanguage::zeusText(69, 0);
            const auto str = std::to_string(e) + " " + emplstr + "  (" + std::to_string(me) + " " + memplstr;
            const auto empl = new eLabel(str, window());
            empl->setFontSizeS();
            empl->setPaddingS();
            empl->fitContent();
            empWid->addWidget(empl);
            empl->align(eAlignment::hcenter);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto eb = dynamic_cast<eEmployingBuilding*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(
                                    window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(eb, title, info,
                                 employmentInfo,
                                 additionalInfo);
            ebWid->initialize(title, info, employmentInfo, eb, additionalInfo);
            wid = ebWid;
        } else if(const auto p = dynamic_cast<ePalace*>(b)) {
            const auto pWid = new eInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(p, title, info,
                                 employmentInfo,
                                 additionalInfo);
            pWid->initialize(title);
            pWid->addText(info);
            if(!employmentInfo.empty()) pWid->addText(employmentInfo);
            wid = pWid;
        } else if(const auto r = dynamic_cast<eRuins*>(b)) {
            const auto rWid = new eInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(r, title, info,
                                 employmentInfo,
                                 additionalInfo);
            rWid->initialize(title);

            const auto wtype = r->wasType();
            if(wtype != eBuildingType::none) {
                const auto name = eBuilding::sNameForBuilding(wtype);
                rWid->addText(name);
            }
            rWid->addText(info);
            wid = rWid;
        } else {
            const auto bWid = new eInfoWidget(window(), this, true, true);
            if(const auto r = dynamic_cast<eRoad*>(b)) {
                if(r->isRoadblock()) {
                    bWid->initialize(eLanguage::zeusText(155, 0));
                    bWid->addText(eLanguage::zeusText(155, 1));
                    wid = bWid;
                }
            }
            if(!wid) {
                const auto title = eBuilding::sNameForBuilding(b);
                bWid->initialize(title);
                wid = bWid;
            }
        }
    }
    if(wid) {
        addWidget(wid);
        wid->setX((width() - mGm->width() - wid->width())/2);
        wid->align(eAlignment::vcenter);
        wid->setCloseAction([this, wid, closeAct]() {
            if(closeAct) closeAct();
            removeWidget(wid);
        });
    }
    return wid;
}

eInfoWidget *eGameWidget::openInfoWidget(const std::vector<eCharacter*> chars) {
    const auto wid = new eCharacterInfoWidget(window(), this);
    wid->initialize(chars);
    if(wid) {
        addWidget(wid);
        wid->setX((width() - mGm->width() - wid->width())/2);
        wid->align(eAlignment::vcenter);
        wid->setCloseAction([this, wid]() {
            removeWidget(wid);
        });
    }
    return wid;
}

bool eGameWidget::hasInfoWidget() const {
    for(const auto w : children()) {
        if(dynamic_cast<eInfoWidget*>(w) && w->visible()) return true;
    }
    return false;
}
