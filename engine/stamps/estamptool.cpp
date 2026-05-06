#include "estamptool.h"

#include "estampblueprint.h"

#include "engine/egameboard.h"
#include "buildings/esmallhouse.h"
#include "buildings/eroad.h"
#include "buildings/eagorabase.h"
#include "buildings/ecommonagora.h"
#include "buildings/evendor.h"
#include "buildings/efoodvendor.h"
#include "buildings/efleecevendor.h"
#include "buildings/eoilvendor.h"
#include "buildings/egranary.h"
#include "buildings/emaintenanceoffice.h"
#include "buildings/epark.h"
#include "buildings/ewarehouse.h"

#include <algorithm>

namespace {
bool isAgoraVendor(const eBuildingType type) {
    return type == eBuildingType::foodVendor ||
           type == eBuildingType::fleeceVendor ||
           type == eBuildingType::oilVendor;
}

stdsptr<eVendor> createVendor(const eBuildingType type,
                              eGameBoard& board,
                              const eCityId cid) {
    switch(type) {
    case eBuildingType::foodVendor:
        return e::make_shared<eFoodVendor>(board, cid);
    case eBuildingType::fleeceVendor:
        return e::make_shared<eFleeceVendor>(board, cid);
    case eBuildingType::oilVendor:
        return e::make_shared<eOilVendor>(board, cid);
    default:
        return nullptr;
    }
}

bool buildCommonAgora(const std::vector<eStampElement>& bp,
                      const int baseX, const int baseY,
                      eGameBoard& board,
                      const eCityId cid) {
    std::vector<eStampElement> vendors;
    for(const auto& elem : bp) {
        if(isAgoraVendor(elem.type)) vendors.push_back(elem);
    }
    if(vendors.size() != 3) return false;

    std::sort(vendors.begin(), vendors.end(),
              [](const eStampElement& a, const eStampElement& b) {
        if(a.dy != b.dy) return a.dy < b.dy;
        return a.dx < b.dx;
    });

    const int y0 = vendors[0].dy;
    if(vendors[1].dy != y0 || vendors[2].dy != y0) return false;
    if(vendors[1].dx != vendors[0].dx + 2 ||
       vendors[2].dx != vendors[0].dx + 4) return false;

    const int rx = baseX + vendors[0].dx;
    const int ry = baseY + y0 - 2;
    const auto agora = e::make_shared<eCommonAgora>(
                           eAgoraOrientation::bottomLeft, board, cid);

    for(int x = rx; x < rx + 6; x++) {
        const auto t = board.tile(x, ry);
        if(!t) return false;
        if(!t->hasRoad()) return false;
    }

    for(int x = rx; x < rx + 6; x++) {
        const auto t = board.tile(x, ry);
        const auto ub = t ? t->underBuilding() : nullptr;
        const auto r = dynamic_cast<eRoad*>(ub);
        if(r) r->setUnderAgora(agora.get());
    }

    agora->setTileRect(SDL_Rect{rx, ry, 6, 3});
    agora->setCenterTile(board.tile(rx + 3, ry));
    agora->fillSpaces();

    const auto agoraRef = agora->ref<eAgoraBase>();
    for(int i = 0; i < 3; i++) {
        const auto vendor = createVendor(vendors[i].type, board, cid);
        if(!vendor) continue;
        vendor->setAgora(agoraRef);
        agora->setBuilding(i, vendor);
    }
    return true;
}
}

eStampTool::eStampTool() :
    mBlueprint(eDefaultStampBlueprint()) {}

void eStampTool::gameAnchorOffset(const eBuildingType type, int& dx, int& dy) {
    int sw;
    int sh;
    buildingSize(type, sw, sh);
    dx = 0;
    dy = 0;
    if(sw == 2 && sh == 2) {
        dy = 1;
    } else if(sw == 3 && sh == 3) {
        dx = 1;
        dy = 1;
    } else if(sw == 4 || sh == 4) {
        dx = 1;
        dy = 2;
    } else if(sw == 5 || sh == 5) {
        dx = 2;
        dy = 2;
    } else if(sw == 6 || sh == 6) {
        dx = 2;
        dy = 2;
    }
}

std::vector<eStampElement> eStampTool::transformedBlueprint() const {
    std::vector<eStampElement> result;
    result.reserve(mBlueprint.size());
    for(const auto& elem : mBlueprint) {
        int adx;
        int ady;
        gameAnchorOffset(elem.type, adx, ady);
        int dx = elem.dx + adx;
        int dy = elem.dy + ady;
        if(mRotation == 1) {
            int temp = dx;
            dx = dy;
            dy = -temp;
        } else if(mRotation == 2) {
            dx = -dx;
            dy = -dy;
        } else if(mRotation == 3) {
            int temp = dx;
            dx = -dy;
            dy = temp;
        }
        if(mMirror) dx = -dx;
        result.push_back({elem.type, dx, dy});
    }
    return result;
}

void eStampTool::buildingSize(const eBuildingType type, int& sw, int& sh) {
    switch(type) {
    case eBuildingType::commonHouse:
        sw = 2;
        sh = 2;
        break;
    case eBuildingType::foodVendor:
    case eBuildingType::fleeceVendor:
    case eBuildingType::oilVendor:
    case eBuildingType::maintenanceOffice:
        sw = 2;
        sh = 2;
        break;
    case eBuildingType::park:
    case eBuildingType::roadblock:
        sw = 1;
        sh = 1;
        break;
    case eBuildingType::warehouse:
        sw = 3;
        sh = 3;
        break;
    case eBuildingType::granary:
        sw = 4;
        sh = 4;
        break;
    default:
        sw = 1;
        sh = 1;
        break;
    }
}

void eStampTool::paintPreview(int baseX, int baseY, eGameBoard* board,
                              bool editorMode, eCityId viewedCityId, ePlayerId playerId,
                              const eDrawXY& drawXY, const eDrawTex& drawTex) const {
    const auto bp = transformedBlueprint();
    // Roads first
    for(const auto& elem : bp) {
        if(elem.type != eBuildingType::road) continue;
        const int bx = baseX + elem.dx;
        const int by = baseY + elem.dy;
        const auto btile = board->tile(bx, by);
        if(!btile) continue;
        double rx, ry;
        drawXY(bx, by, rx, ry, 1, 1, btile->altitude());
        const bool can = board->canBuild(bx, by, 1, 1, editorMode, viewedCityId, playerId);
        drawTex(rx, ry, eBuildingType::road, 1, can);
    }
    // Buildings
    for(const auto& elem : bp) {
        if(elem.type == eBuildingType::road) continue;
        const int bx = baseX + elem.dx;
        const int by = baseY + elem.dy;
        const auto btile = board->tile(bx, by);
        if(!btile) continue;
        int sw, sh;
        buildingSize(elem.type, sw, sh);
        double rx, ry;
        drawXY(bx, by, rx, ry, sw, sh, btile->altitude());
        const bool can = board->canBuild(bx, by, sw, sh, editorMode, viewedCityId, playerId);
        drawTex(rx, ry, elem.type, sw, can);
    }
}

bool eStampTool::canBuildAt(int baseX, int baseY, eGameBoard* board,
                            bool editorMode, eCityId viewedCityId, ePlayerId playerId) const {
    for(const auto& elem : transformedBlueprint()) {
        int sw;
        int sh;
        buildingSize(elem.type, sw, sh);
        const int tx = baseX + elem.dx;
        const int ty = baseY + elem.dy;
        if(!board->canBuild(tx, ty, sw, sh, editorMode,
                            viewedCityId, playerId)) {
            return false;
        }
    }
    return true;
}

void eStampTool::buildAt(int baseX, int baseY, eGameBoard* board, eCityId viewedCityId, ePlayerId playerId, bool editorMode) {
    const auto bp = transformedBlueprint();
    for(const auto& elem : bp) {
        if(isAgoraVendor(elem.type)) continue;
        int sw;
        int sh;
        buildingSize(elem.type, sw, sh);
        const int tx = baseX + elem.dx;
        const int ty = baseY + elem.dy;
        eGameBoard::eBuildingCreator creator;
        switch(elem.type) {
        case eBuildingType::commonHouse:
            creator = [board, viewedCityId]() { return e::make_shared<eSmallHouse>(*board, viewedCityId); };
            break;
        case eBuildingType::road:
            creator = [board, viewedCityId]() { return e::make_shared<eRoad>(*board, viewedCityId); };
            break;
        case eBuildingType::roadblock:
            creator = [board, viewedCityId]() {
                const auto r = e::make_shared<eRoad>(*board, viewedCityId);
                r->setRoadblock(true);
                return r;
            };
            break;
        case eBuildingType::foodVendor:
            creator = [board, viewedCityId]() { return e::make_shared<eFoodVendor>(*board, viewedCityId); };
            break;
        case eBuildingType::fleeceVendor:
            creator = [board, viewedCityId]() { return e::make_shared<eFleeceVendor>(*board, viewedCityId); };
            break;
        case eBuildingType::oilVendor:
            creator = [board, viewedCityId]() { return e::make_shared<eOilVendor>(*board, viewedCityId); };
            break;
        case eBuildingType::granary:
            creator = [board, viewedCityId]() { return e::make_shared<eGranary>(*board, viewedCityId); };
            break;
        case eBuildingType::maintenanceOffice:
            creator = [board, viewedCityId]() { return e::make_shared<eMaintenanceOffice>(*board, viewedCityId); };
            break;
        case eBuildingType::park:
            creator = [board, viewedCityId]() { return e::make_shared<ePark>(*board, viewedCityId); };
            break;
        case eBuildingType::warehouse:
            creator = [board, viewedCityId]() { return e::make_shared<eWarehouse>(*board, viewedCityId); };
            break;
        default:
            continue;
        }
        board->build(tx, ty, sw, sh, viewedCityId, playerId, editorMode, creator);
    }
    buildCommonAgora(bp, baseX, baseY, *board, viewedCityId);
}
