#include "game-widget.h"
#include "infowidgets/einfowidget.h"
#include "infowidgets/eemployingbuildinginfowidget.h"

#include "elanguage.h"

#include "buildings/allbuildings.h"
#include "buildings/efarmbase.h"
#include "buildings/eshepherbuildingbase.h"
#include "enumbers.h"

#include "engine/edate.h"

#include "evectorhelpers.h"
#include "infowidgets/eagorainfowidget.h"
#include "infowidgets/echaracterinfowidget.h"
#include "infowidgets/common-house-info-widget.h"
#include "infowidgets/eheroshallinfowidget.h"
#include "infowidgets/ehippodromeinfowidget.h"
#include "infowidgets/esanctuaryinfowidget.h"
#include "infowidgets/storage-info-widget.h"
#include "infowidgets/trade-post-info-widget.h"
#include "infowidgets/etriremewharfinfowidget.h"

#include "engine/game-board.h"

eInfoWidget* GameWidget::openInfoWidget(eBuilding* const b) {
    eInfoWidget* wid = nullptr;
    eAction closeAct;
    if(const auto house = dynamic_cast<eHouseBase*>(b)) {
        const auto w = new CommonHouseInfoWidget(window(), this);
        w->initialize(house);
        wid = w;
    } else if(const auto stor = dynamic_cast<TradePost*>(b)) {
        const auto storWid = new TradePostInfoWidget(window(), this);
        const auto prevNext = [this, storWid, stor, closeAct](const bool next) {
            const auto pid = stor->playerId();
            const auto& board = stor->getBoard();
            std::vector<TradePost*> posts;
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
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
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
            constexpr int maxYearly = 8;
            const double eff = maxYearly > 0 ? (double)fb->producedThisYear() / maxYearly : 0;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(fb->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(fb, employmentInfo);
            const eMonth hm = fb->nextHarvestMonth();
            std::string harvestStr = eLanguage::zeusText(group, 14) + " " + eMonthHelper::name(hm) + ".";
            ebWid->addText(harvestStr);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto pb = dynamic_cast<eProcessingBuilding*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
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
            case eBuildingType::growersLodge:
                group = 179;
                break;

            case eBuildingType::chariotFactory:
                group = 281;
                break;
            default:
                group = 122;
                break;
            }
            std::string storedStr = eLanguage::zeusText(group, 12);
            if(storedStr.empty()) {
                storedStr = eLanguage::zeusText(group, 11);
            }
            if(!storedStr.empty()) {
                storedStr += " " + std::to_string(pb->rawCount());
                ebWid->addText(storedStr);
            }
            std::string prodStr = eLanguage::zeusText(group, 2) + " " + std::to_string(p) + "% " + eLanguage::zeusText(group, 3);
            ebWid->addText(prodStr);
            const int maxYearly = (360 * eNumbers::sDayLength) / pb->processWaitTime();
            const double eff = maxYearly > 0 ? (double)pb->producedThisYear() / maxYearly : 0;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(pb->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(pb, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto sb = dynamic_cast<eShepherBuildingBase*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(sb, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            const int group = sb->type() == eBuildingType::dairy ? 121 : 123;
            const std::string holdsStr = eLanguage::zeusText(group, 2) + " " + std::to_string(sb->resource()) + " " + eLanguage::zeusText(group, 3);
            ebWid->addText(holdsStr);
            constexpr int maxYearly = 8;
            const double eff = (double)sb->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(sb->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(sb, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto gl = dynamic_cast<eGrowersLodge*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(gl, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            std::string line;
            if(gl->type() == eGrowerType::oranges) {
                line = "Oranges: " + std::to_string(gl->orangeUnits());
            } else {
                std::string storedStr = eLanguage::zeusText(179, 11); // olives
                if(!storedStr.empty()) {
                    line += storedStr + " " + std::to_string(gl->oliveUnits());
                }
                storedStr = eLanguage::zeusText(179, 12); // grapes
                if(!storedStr.empty()) {
                    if(!line.empty()) line += "       ";
                    line += storedStr + " " + std::to_string(gl->grapeUnits());
                }
            }
            if(!line.empty()) {
                ebWid->addText(line);
            }
            constexpr int maxYearly = 20;
            const int producedLoads = gl->producedThisYear() / eGrowersLodge::sUnitsPerLoad;
            const double eff = (double)producedLoads / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(producedLoads) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(gl, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto hl = dynamic_cast<eHuntingLodge*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
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
            ebWid->addEmploymentWidget(hl, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto fy = dynamic_cast<eFishery*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
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
            ebWid->addEmploymentWidget(fy, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto tm = dynamic_cast<eTimberMill*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(tm, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            const std::string holdsStr = eLanguage::zeusText(120, 2) + " " + std::to_string(tm->resource()) + " " + eLanguage::zeusText(120, 3);
            ebWid->addText(holdsStr);
            constexpr int maxYearly = 13;
            const double eff = (double)tm->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(tm->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(tm, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto ms = dynamic_cast<eMasonryShop*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(ms, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            const std::string holdsStr = eLanguage::zeusText(118, 2) + " " + std::to_string(ms->resource()) + " " + eLanguage::zeusText(118, 3);
            ebWid->addText(holdsStr);
            constexpr int maxYearly = 12;
            const double eff = (double)ms->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(ms->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(ms, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto bm = dynamic_cast<eBlackMarbleWorkshop*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(bm, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            const std::string holdsStr = eLanguage::zeusText(169, 2) + " " + std::to_string(bm->resource()) + " " + eLanguage::zeusText(169, 3);
            ebWid->addText(holdsStr);
            constexpr int maxYearly = 12;
            const double eff = (double)bm->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(bm->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(bm, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto rf = dynamic_cast<eRefinery*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(rf, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            const std::string holdsStr = eLanguage::zeusText(109, 2) + " " + std::to_string(rf->resource()) + " " + eLanguage::zeusText(109, 3);
            ebWid->addText(holdsStr);
            constexpr int maxYearly = 12;
            const double eff = (double)rf->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(rf->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(rf, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto fd = dynamic_cast<eFoundry*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(fd, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            const std::string holdsStr = eLanguage::zeusText(117, 2) + " " + std::to_string(fd->resource()) + " " + eLanguage::zeusText(117, 3);
            ebWid->addText(holdsStr);
            constexpr int maxYearly = 12;
            const double eff = (double)fd->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(fd->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(fd, employmentInfo);
            ebWid->addText(info);
            wid = ebWid;
        } else if(const auto mt = dynamic_cast<eMint*>(b)) {
            const auto ebWid = new eEmployingBuildingInfoWidget(window(), this, true, true);
            std::string title;
            std::string info;
            std::string employmentInfo;
            std::string additionalInfo;
            eBuilding::sInfoText(mt, title, info, employmentInfo, additionalInfo);
            ebWid->initialize(title);
            const std::string holdsStr = eLanguage::zeusText(119, 2) + " " + std::to_string(mt->resource()) + " " + eLanguage::zeusText(119, 3);
            ebWid->addText(holdsStr);
            constexpr int maxYearly = 12;
            const double eff = (double)mt->producedThisYear() / maxYearly;
            const int effPercent = static_cast<int>(eff * 100 + 0.5);
            std::string effStr = "Efficiency: " + std::to_string(effPercent) + "% (" + std::to_string(mt->producedThisYear()) + "/" + std::to_string(maxYearly) + ")";
            ebWid->addText(effStr);
            ebWid->addEmploymentWidget(mt, employmentInfo);
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
        } else if(const auto rb = dynamic_cast<eResourceBuilding*>(b)) {
            const auto rbWid = new eInfoWidget(window(), this, true, true);
            const auto title = eBuilding::sNameForBuilding(b);
            rbWid->initialize(title);
            int group = 115;
            eMonth harvestStart = eMonth::january;
            eMonth harvestEnd = eMonth::march;
            switch(rb->type()) {
            // xml group 115 = Olive Tree. ids 2,3 = "Fruit is"/"ripe.", id 14 = "The next olive harvest is in"
            case eResourceBuildingType::oliveTree:
                group = 115;
                harvestStart = eMonth::january;
                harvestEnd = eMonth::march;
                break;
            // xml group 116 = Grapevine. ids 2,3 = "Fruit is"/"ripe.", id 14 = "The next grape harvest is in"
            case eResourceBuildingType::vine:
                group = 116;
                harvestStart = eMonth::october;
                harvestEnd = eMonth::december;
                break;
            // xml group 107 = Orange Tree. ids 2,3 = "Oranges are"/"ripe.", id 14 = "The next orange harvest is in"
            case eResourceBuildingType::orangeTree:
                group = 107;
                harvestStart = eMonth::january;
                harvestEnd = eMonth::march;
                break;
            }
            const int pct = rb->ripe() * 100 / 5;
            // xml strings (groupId, 2) + (groupId, 3) â†’ e.g. "Fruit is 80% ripe."
            const std::string ripeStr = eLanguage::zeusText(group, 2) + " " +
                                        std::to_string(pct) + "% " +
                                        eLanguage::zeusText(group, 3);
            rbWid->addText(ripeStr);

            const auto curMonth = rb->getBoard().date().month();
            const int cm = static_cast<int>(curMonth);
            const int hs = static_cast<int>(harvestStart);
            const int he = static_cast<int>(harvestEnd);
            const bool inSeason = cm >= hs && cm <= he;
            // xml group 160 = month names, ids 0..11 = January..December
            const std::string startName = eLanguage::zeusText(160, hs);
            const std::string endName = eLanguage::zeusText(160, he);
            std::string seasonStr;
            if(inSeason) {
                // xml (group, 14) = "The next X harvest is in" â€” reused here to label the season
                seasonStr = "Harvest available now (" + startName +
                            "-" + endName + ")";
            } else {
                // xml (group, 14) = "The next X harvest is in <start month>"
                seasonStr = eLanguage::zeusText(group, 14) + " " + startName;
            }
            rbWid->addText(seasonStr);
            wid = rbWid;
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

eInfoWidget *GameWidget::openInfoWidget(const std::vector<eCharacter*> chars) {
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

bool GameWidget::hasInfoWidget() const {
    for(const auto w : children()) {
        if(dynamic_cast<eInfoWidget*>(w) && w->visible()) return true;
    }
    return false;
}
