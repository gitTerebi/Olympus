#include "estamptool.h"

#include "estampblueprint.h"

#include "engine/e-game-board.h"
#include "engine/difficulty.h"
#include "buildings/ebuilding.h"
#include "buildings/eagorabase.h"

#include <algorithm>

eStampTool::eStampTool() = default;

bool eStampTool::setTemplate(const std::string& name,
                              const std::string& path) {
    std::vector<eStampElement> bp;
    if(!eReadStampBlueprint(path, bp)) return false;
    mBlueprint = bp;
    mTemplateName = name;
    return true;
}

int agoraWidth(const eAgoraOrientation o) {
    switch(o) {
    case eAgoraOrientation::bottomLeft:
    case eAgoraOrientation::topRight:
        return 6;
    case eAgoraOrientation::bottomRight:
    case eAgoraOrientation::topLeft:
        return 3;
    }
}

int agoraHeight(const eAgoraOrientation o) {
    switch(o) {
    case eAgoraOrientation::bottomLeft:
    case eAgoraOrientation::topRight:
        return 3;
    case eAgoraOrientation::bottomRight:
    case eAgoraOrientation::topLeft:
        return 6;
    }
}

void eStampTool::transformPoint(int& dx, int& dy) const {
    if(mRotation == 1) {
        const int temp = dx;
        dx = dy;
        dy = -temp;
    } else if(mRotation == 2) {
        dx = -dx;
        dy = -dy;
    } else if(mRotation == 3) {
        const int temp = dx;
        dx = -dy;
        dy = temp;
    }
    if(mMirror) dx = -dx;
}

eStampElement eStampTool::transformedElement(const eStampElement& elem) const {
    if(elem.type == eBuildingType::commonAgora && elem.id >= 0) {
        auto o = static_cast<eAgoraOrientation>(elem.id);
        if(mMirror) {
            if(o == eAgoraOrientation::bottomRight) {
                o = eAgoraOrientation::topLeft;
            } else if(o == eAgoraOrientation::topLeft) {
                o = eAgoraOrientation::bottomRight;
            }
        }
        for(int i = 0; i < mRotation; i++) {
            switch(o) {
            case eAgoraOrientation::bottomLeft:
                o = eAgoraOrientation::bottomRight;
                break;
            case eAgoraOrientation::bottomRight:
                o = eAgoraOrientation::topRight;
                break;
            case eAgoraOrientation::topRight:
                o = eAgoraOrientation::topLeft;
                break;
            case eAgoraOrientation::topLeft:
                o = eAgoraOrientation::bottomLeft;
                break;
            }
        }

        const int sw = agoraWidth(static_cast<eAgoraOrientation>(elem.id));
        const int sh = agoraHeight(static_cast<eAgoraOrientation>(elem.id));
        int minX = 0;
        int minY = 0;
        int maxX = 0;
        int maxY = 0;
        bool first = true;
        for(int x = elem.dx; x < elem.dx + sw; x++) {
            for(int y = elem.dy; y < elem.dy + sh; y++) {
                int tx = x;
                int ty = y;
                transformPoint(tx, ty);
                if(first) {
                    minX = maxX = tx;
                    minY = maxY = ty;
                    first = false;
                } else {
                    minX = std::min(minX, tx);
                    maxX = std::max(maxX, tx);
                    minY = std::min(minY, ty);
                    maxY = std::max(maxY, ty);
                }
            }
        }
        return {elem.type, minX, minY, static_cast<int>(o)};
    }

    int sw;
    int sh;
    elementSize(elem, sw, sh);
    int ox;
    int oy;
    buildingDrawOffset(sw, sh, ox, oy);

    const int x0 = elem.dx + ox;
    const int y0 = elem.dy + oy;
    int minX = 0;
    int minY = 0;
    int maxX = 0;
    int maxY = 0;
    bool first = true;
    for(int x = x0; x < x0 + sw; x++) {
        for(int y = y0; y < y0 + sh; y++) {
            int tx = x;
            int ty = y;
            transformPoint(tx, ty);
            if(first) {
                minX = maxX = tx;
                minY = maxY = ty;
                first = false;
            } else {
                minX = std::min(minX, tx);
                maxX = std::max(maxX, tx);
                minY = std::min(minY, ty);
                maxY = std::max(maxY, ty);
            }
        }
    }

    const int rw = maxX - minX + 1;
    const int rh = maxY - minY + 1;
    buildingDrawOffset(rw, rh, ox, oy);
    return {elem.type, minX - ox, minY - oy, elem.id};
}

std::vector<eStampElement> eStampTool::transformedBlueprint() const {
    std::vector<eStampElement> result;
    result.reserve(mBlueprint.size());
    for(const auto& elem : mBlueprint) {
        result.push_back(transformedElement(elem));
    }
    return result;
}

std::vector<eStampBuildCommand> eStampTool::buildCommands() const {
    const auto bp = transformedBlueprint();
    std::vector<eStampBuildCommand> result;
    result.reserve(bp.size() + 4);

    const auto isVendor = [](const eBuildingType type) {
        return type == eBuildingType::foodVendor ||
               type == eBuildingType::fleeceVendor ||
               type == eBuildingType::oilVendor ||
               type == eBuildingType::armsVendor ||
               type == eBuildingType::wineVendor ||
               type == eBuildingType::horseTrainer;
    };
    const auto append = [&](const eBuildingMode mode,
                            const eStampElement& elem) {
        result.push_back({mode, elem.dx, elem.dy});
    };
    const auto anchorTopLeft = [](const int dx, const int dy,
                                  const int sw, const int sh,
                                  int& x0, int& y0) {
        x0 = dx;
        y0 = dy;
        if(sw == 2 && sh == 2) {
            y0 -= 1;
        } else if(sw == 3 && sh == 3) {
            x0 -= 1;
            y0 -= 1;
        } else if(sw == 4 || sh == 4) {
            x0 -= 1;
            y0 -= 2;
        } else if(sw == 5 || sh == 5) {
            x0 -= 2;
            y0 -= 2;
        } else if(sw == 6 || sh == 6) {
            x0 -= 2;
            y0 -= 2;
        }
    };
    const auto isSourceRoad = [this](const int dx, const int dy) {
        return std::find_if(mBlueprint.begin(), mBlueprint.end(),
                            [dx, dy](const eStampElement& elem) {
            return elem.type == eBuildingType::road &&
                   elem.dx == dx && elem.dy == dy;
        }) != mBlueprint.end();
    };
    const auto transformedRoads = [this](const std::vector<std::pair<int, int>>& roads) {
        std::vector<std::pair<int, int>> result;
        result.reserve(roads.size());
        for(const auto& road : roads) {
            const auto elem = transformedElement(
                                  {eBuildingType::road, road.first, road.second});
            result.push_back({elem.dx, elem.dy});
        }
        return result;
    };
    const auto transformedCells = [this](const std::vector<std::pair<int, int>>& cells) {
        std::vector<std::pair<int, int>> result;
        result.reserve(cells.size());
        for(const auto& cell : cells) {
            int tx = cell.first;
            int ty = cell.second;
            transformPoint(tx, ty);
            result.push_back({tx, ty});
        }
        return result;
    };
    const auto isRoad = [&bp](const int dx, const int dy) {
        return std::find_if(bp.begin(), bp.end(), [dx, dy](const eStampElement& elem) {
            return elem.type == eBuildingType::road &&
                   elem.dx == dx && elem.dy == dy;
        }) != bp.end();
    };
    const auto appendAgoraFromRun = [&](const int dx, const int dy,
                                        const eAgoraOrientation orientation) {
        result.push_back({eBuildingMode::commonAgora, dx, dy,
                          static_cast<int>(orientation)});
    };
    const auto appendExplicitAgora = [&](const eStampElement& elem) {
        const auto orientation = static_cast<eAgoraOrientation>(elem.id);
        const auto transformed = transformedElement(elem);
        std::vector<std::pair<int, int>> roads;
        std::vector<std::pair<int, int>> spaces;
        switch(orientation) {
        case eAgoraOrientation::bottomLeft:
            for(int i = 0; i < 6; i++) {
                roads.push_back({elem.dx + i, elem.dy});
                spaces.push_back({elem.dx + i, elem.dy + 1});
                spaces.push_back({elem.dx + i, elem.dy + 2});
            }
            break;
        case eAgoraOrientation::topRight:
            for(int i = 0; i < 6; i++) {
                roads.push_back({elem.dx + i, elem.dy + 2});
                spaces.push_back({elem.dx + i, elem.dy});
                spaces.push_back({elem.dx + i, elem.dy + 1});
            }
            break;
        case eAgoraOrientation::bottomRight:
            for(int i = 0; i < 6; i++) {
                roads.push_back({elem.dx, elem.dy + i});
                spaces.push_back({elem.dx + 1, elem.dy + i});
                spaces.push_back({elem.dx + 2, elem.dy + i});
            }
            break;
        case eAgoraOrientation::topLeft:
            for(int i = 0; i < 6; i++) {
                roads.push_back({elem.dx + 2, elem.dy + i});
                spaces.push_back({elem.dx, elem.dy + i});
                spaces.push_back({elem.dx + 1, elem.dy + i});
            }
            break;
        }
        result.push_back({eBuildingMode::commonAgora, transformed.dx,
                          transformed.dy, -1, transformedRoads(roads),
                          transformedCells(spaces)});
    };
    const auto appendAgoraForVendor = [&](const eStampElement& elem) {
        int sw;
        int sh;
        buildingSize(elem.type, sw, sh);
        int x0;
        int y0;
        anchorTopLeft(elem.dx, elem.dy, sw, sh, x0, y0);
        const auto transformed = transformedElement(elem);
        const auto appendAgoraRoads = [&](const std::vector<std::pair<int, int>>& roads,
                                          const std::vector<std::pair<int, int>>& spaces) {
            result.push_back({eBuildingMode::commonAgora, transformed.dx,
                              transformed.dy, -1, transformedRoads(roads),
                              transformedCells(spaces)});
        };
        for(int start = x0; start <= x0 + sw - 1; start++) {
            bool run = true;
            std::vector<std::pair<int, int>> roads;
            std::vector<std::pair<int, int>> spaces;
            for(int i = 0; i < 6; i++) {
                const int rx = start + i;
                const int ry = y0 - 1;
                if(!isSourceRoad(rx, ry)) {
                    run = false;
                    break;
                }
                roads.push_back({rx, ry});
                spaces.push_back({rx, y0});
                spaces.push_back({rx, y0 + 1});
            }
            if(run) {
                appendAgoraRoads(roads, spaces);
                return true;
            }
        }
        for(int start = x0; start <= x0 + sw - 1; start++) {
            bool run = true;
            std::vector<std::pair<int, int>> roads;
            std::vector<std::pair<int, int>> spaces;
            for(int i = 0; i < 6; i++) {
                const int rx = start + i;
                const int ry = y0 + sh;
                if(!isSourceRoad(rx, ry)) {
                    run = false;
                    break;
                }
                roads.push_back({rx, ry});
                spaces.push_back({rx, y0 + sh - 2});
                spaces.push_back({rx, y0 + sh - 1});
            }
            if(run) {
                appendAgoraRoads(roads, spaces);
                return true;
            }
        }
        for(int start = y0; start <= y0 + sh - 1; start++) {
            bool run = true;
            std::vector<std::pair<int, int>> roads;
            std::vector<std::pair<int, int>> spaces;
            for(int i = 0; i < 6; i++) {
                const int rx = x0 - 1;
                const int ry = start + i;
                if(!isSourceRoad(rx, ry)) {
                    run = false;
                    break;
                }
                roads.push_back({rx, ry});
                spaces.push_back({x0, ry});
                spaces.push_back({x0 + 1, ry});
            }
            if(run) {
                appendAgoraRoads(roads, spaces);
                return true;
            }
        }
        for(int start = y0; start <= y0 + sh - 1; start++) {
            bool run = true;
            std::vector<std::pair<int, int>> roads;
            std::vector<std::pair<int, int>> spaces;
            for(int i = 0; i < 6; i++) {
                const int rx = x0 + sw;
                const int ry = start + i;
                if(!isSourceRoad(rx, ry)) {
                    run = false;
                    break;
                }
                roads.push_back({rx, ry});
                spaces.push_back({x0 + sw - 2, ry});
                spaces.push_back({x0 + sw - 1, ry});
            }
            if(run) {
                appendAgoraRoads(roads, spaces);
                return true;
            }
        }
        return false;
    };

    for(const auto& elem : bp) {
        if(elem.type != eBuildingType::road &&
           elem.type != eBuildingType::roadblock) continue;
        append(eBuildingMode::road, elem);
    }

    for(const auto& elem : bp) {
        if(elem.type != eBuildingType::roadblock) continue;
        append(eBuildingMode::roadblock, elem);
    }

    const bool hasExplicitAgora = std::find_if(
        mBlueprint.begin(), mBlueprint.end(), [](const eStampElement& elem) {
            return elem.type == eBuildingType::commonAgora && elem.id >= 0;
        }) != mBlueprint.end();

    for(const auto& elem : mBlueprint) {
        if(elem.type != eBuildingType::commonAgora || elem.id < 0) continue;
        appendExplicitAgora(elem);
    }

    if(!hasExplicitAgora) {
        for(const auto& elem : mBlueprint) {
            if(!isVendor(elem.type)) continue;
            if(appendAgoraForVendor(elem)) continue;
            const auto transformed = transformedElement(elem);
            for(int dy = 1; dy <= 2; dy++) {
                result.push_back({eBuildingMode::commonAgora,
                                  transformed.dx, transformed.dy - dy});
                result.push_back({eBuildingMode::commonAgora,
                                  transformed.dx, transformed.dy + dy});
            }
            for(int dx = 1; dx <= 2; dx++) {
                result.push_back({eBuildingMode::commonAgora,
                                  transformed.dx - dx, transformed.dy});
                result.push_back({eBuildingMode::commonAgora,
                                  transformed.dx + dx, transformed.dy});
            }
        }
    }

    for(const auto& elem : bp) {
        if(!isVendor(elem.type)) continue;
        const auto mode = eBuildingModeHelpers::fromBuildingType(elem.type);
        if(mode == eBuildingMode::none) continue;
        append(mode, elem);
    }

    for(const auto& elem : bp) {
        if(elem.type == eBuildingType::road ||
           elem.type == eBuildingType::roadblock ||
           isVendor(elem.type)) continue;
        const auto mode = eBuildingModeHelpers::fromBuildingType(elem.type);
        if(mode == eBuildingMode::none) continue;
        append(mode, elem);
    }

    return result;
}

int eStampTool::estimatedCost(const Difficulty diff) const {
    int cost = 0;
    bool implicitAgoraCounted = false;
    for(const auto& cmd : buildCommands()) {
        eBuildingType bt = eBuildingType::none;
        if(cmd.mode == eBuildingMode::commonAgora) {
            if(cmd.agoraRect) {
                bt = eBuildingType::commonAgora;
            } else {
                if(implicitAgoraCounted) continue;
                implicitAgoraCounted = true;
                bt = eBuildingType::commonAgora;
            }
        } else {
            bt = eBuildingModeHelpers::toBuildingType(cmd.mode);
        }
        if(bt == eBuildingType::none) continue;
        cost += eDifficultyHelpers::buildingCost(diff, bt);
    }
    return cost;
}

void eStampTool::buildingSize(const eBuildingType type, int& sw, int& sh) {
    switch(type) {
    case eBuildingType::commonHouse:
    case eBuildingType::foodVendor:
    case eBuildingType::fleeceVendor:
    case eBuildingType::oilVendor:
    case eBuildingType::armsVendor:
    case eBuildingType::wineVendor:
    case eBuildingType::horseTrainer:
    case eBuildingType::maintenanceOffice:
    case eBuildingType::fountain:
    case eBuildingType::gazebo:
    case eBuildingType::taxOffice:
    case eBuildingType::podium:
    case eBuildingType::bibliotheke:
    case eBuildingType::cardingShed:
    case eBuildingType::dairy:
    case eBuildingType::growersLodge:
    case eBuildingType::huntingLodge:
    case eBuildingType::fishery:
    case eBuildingType::urchinQuay:
    case eBuildingType::timberMill:
    case eBuildingType::masonryShop:
    case eBuildingType::foundry:
    case eBuildingType::mint:
    case eBuildingType::olivePress:
    case eBuildingType::winery:
    case eBuildingType::artisansGuild:
    case eBuildingType::tower:
    case eBuildingType::armory:
    case eBuildingType::pier:
        sw = 2;
        sh = 2;
        break;
    case eBuildingType::warehouse:
    case eBuildingType::college:
    case eBuildingType::gymnasium:
    case eBuildingType::commemorative:
    case eBuildingType::hedgeMaze:
    case eBuildingType::dramaSchool:
    case eBuildingType::horseRanch:
    case eBuildingType::inventorsWorkshop:
    case eBuildingType::university:
    case eBuildingType::wheatFarm:
    case eBuildingType::carrotsFarm:
    case eBuildingType::onionsFarm:
        sw = 3;
        sh = 3;
        break;
    case eBuildingType::granary:
    case eBuildingType::hospital:
    case eBuildingType::eliteHousing:
    case eBuildingType::fishPond:
    case eBuildingType::tradePost:
    case eBuildingType::horseRanchEnclosure:
    case eBuildingType::laboratory:
        sw = 4;
        sh = 4;
        break;
    case eBuildingType::theater:
    case eBuildingType::observatory:
        sw = 5;
        sh = 5;
        break;
    case eBuildingType::park:
    case eBuildingType::roadblock:
    case eBuildingType::bench:
    default:
        sw = 1;
        sh = 1;
        break;
    }
}

void eStampTool::elementSize(const eStampElement& elem, int& sw, int& sh) {
    if(elem.type == eBuildingType::commonAgora && elem.id >= 0) {
        const auto o = static_cast<eAgoraOrientation>(elem.id);
        sw = agoraWidth(o);
        sh = agoraHeight(o);
    } else {
        buildingSize(elem.type, sw, sh);
    }
}

void eStampTool::buildingDrawOffset(const int sw, const int sh,
                                    int& dx, int& dy) {
    dx = 0;
    dy = 0;
    if(sw == 2 && sh == 2) {
        dy = -1;
    } else if(sw == 3 && sh == 3) {
        dx = -1;
        dy = -1;
    } else if(sw == 4 || sh == 4) {
        dx = -1;
        dy = -2;
    } else if(sw == 5 || sh == 5) {
        dx = -2;
        dy = -2;
    } else if(sw == 6 || sh == 6) {
        dx = -2;
        dy = -2;
    }
}

void eStampTool::paintPreview(int baseX, int baseY, GameBoard* board,
                              bool editorMode, eCityId viewedCityId, ePlayerId playerId,
                              const eDrawXY& drawXY, const eDrawTex& drawTex,
                              const eDrawAgora& drawAgora,
                              const eDrawStampAgora& drawStampAgora) const {
    auto bp = transformedBlueprint();
    const auto cmds = buildCommands();
    // Sort by screen Y for correct painter's algorithm z-order under any view rotation
    std::sort(bp.begin(), bp.end(), [&](const eStampElement& a, const eStampElement& b) {
        double rxa, rya, rxb, ryb;
        drawXY(baseX + a.dx, baseY + a.dy, rxa, rya, 1, 1, 0);
        drawXY(baseX + b.dx, baseY + b.dy, rxb, ryb, 1, 1, 0);
        return rya < ryb;
    });
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
    // Keep stamp ghost order matched to build order: roads, agora, then vendors/buildings.
    for(const auto& cmd : cmds) {
        if(cmd.mode != eBuildingMode::commonAgora) continue;
        if(cmd.agoraRect && cmd.agoraOrientation >= 0) {
            drawAgora(baseX + cmd.dx, baseY + cmd.dy, cmd.agoraOrientation);
        } else {
            drawStampAgora(cmd);
        }
    }
    // Buildings (already sorted by screen Y — back to front)
    for(const auto& elem : bp) {
        if(elem.type == eBuildingType::road ||
           elem.type == eBuildingType::commonAgora) continue;
        const int bx = baseX + elem.dx;
        const int by = baseY + elem.dy;
        const auto btile = board->tile(bx, by);
        if(!btile) continue;
        int sw, sh;
        elementSize(elem, sw, sh);
        double rx, ry;
        drawXY(bx, by, rx, ry, sw, sh, btile->altitude());
        const bool fertile = elem.type == eBuildingType::wheatFarm ||
                             elem.type == eBuildingType::carrotsFarm ||
                             elem.type == eBuildingType::onionsFarm;
        const bool can = board->canBuild(bx, by, sw, sh, editorMode,
                                         viewedCityId, playerId, fertile);
        drawTex(rx, ry, elem.type, sw, can);
    }
}
