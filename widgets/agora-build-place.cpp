#include "agora-build-place.h"

#include "engine/e-game-board.h"
#include "buildings/eroad.h"
#include "engine/etile.h"

bool agoraRoadTile(eTile* const t) {
    if(!t) return false;
    if(!t->hasRoad()) return false;
    const auto ub = t->underBuilding();
    if(!ub) return false;
    const auto r = static_cast<eRoad*>(ub);
    return !r->underAgora();
}

static bool agoraSpaceTile(GameBoard* board, bool editorMode,
                            int x, int y, eCityId cid, ePlayerId pid) {
    const auto t = board->tile(x, y);
    if(!t) return false;
    return board->canBuild(x, y, 1, 1, editorMode, cid, pid) || t->hasRoad();
}

std::vector<eTile*> agoraBuildPlaceBR(GameBoard* board, bool editorMode,
                                      eTile* const tile,
                                      eCityId cid, ePlayerId pid) {
    if(!agoraRoadTile(tile)) return {};
    const auto tr1 = tile->topRight<eTile>();
    if(!agoraRoadTile(tr1)) return {};
    const auto tr2 = tr1->topRight<eTile>();
    if(!agoraRoadTile(tr2)) return {};
    const auto tr3 = tr2->topRight<eTile>();
    if(!agoraRoadTile(tr3)) return {};
    const auto tr4 = tr3->topRight<eTile>();
    if(!agoraRoadTile(tr4)) return {};
    const auto tr5 = tr4->topRight<eTile>();
    if(!agoraRoadTile(tr5)) return {};
    std::vector<eTile*> tiles;
    tiles.push_back(tr5);
    tiles.push_back(tr4);
    tiles.push_back(tr3);
    tiles.push_back(tr2);
    tiles.push_back(tr1);
    tiles.push_back(tile);
    const int iMin = tile->x() + 1;
    const int iMax = iMin + 2;
    const int jMin = tile->y() - 5;
    const int jMax = jMin + 6;
    for(int i = iMin; i < iMax; i++) {
        for(int j = jMin; j < jMax; j++) {
            if(!agoraSpaceTile(board, editorMode, i, j, cid, pid)) return {};
            tiles.push_back(board->tile(i, j));
        }
    }
    return tiles;
}

std::vector<eTile*> agoraBuildPlaceTL(GameBoard* board, bool editorMode,
                                      eTile* const tile,
                                      eCityId cid, ePlayerId pid) {
    if(!agoraRoadTile(tile)) return {};
    const auto tr1 = tile->topRight<eTile>();
    if(!agoraRoadTile(tr1)) return {};
    const auto tr2 = tr1->topRight<eTile>();
    if(!agoraRoadTile(tr2)) return {};
    const auto tr3 = tr2->topRight<eTile>();
    if(!agoraRoadTile(tr3)) return {};
    const auto tr4 = tr3->topRight<eTile>();
    if(!agoraRoadTile(tr4)) return {};
    const auto tr5 = tr4->topRight<eTile>();
    if(!agoraRoadTile(tr5)) return {};
    std::vector<eTile*> tiles;
    tiles.push_back(tr5);
    tiles.push_back(tr4);
    tiles.push_back(tr3);
    tiles.push_back(tr2);
    tiles.push_back(tr1);
    tiles.push_back(tile);
    const int iMin = tile->x() - 3;
    const int iMax = iMin + 2;
    const int jMin = tile->y() - 5;
    const int jMax = jMin + 6;
    for(int i = iMax; i > iMin; i--) {
        for(int j = jMin; j < jMax; j++) {
            if(!agoraSpaceTile(board, editorMode, i, j, cid, pid)) return {};
            tiles.push_back(board->tile(i, j));
        }
    }
    return tiles;
}

std::vector<eTile*> agoraBuildPlaceBL(GameBoard* board, bool editorMode,
                                      eTile* const tile,
                                      eCityId cid, ePlayerId pid) {
    if(!agoraRoadTile(tile)) return {};
    const auto tl1 = tile->topLeft<eTile>();
    if(!agoraRoadTile(tl1)) return {};
    const auto tl2 = tl1->topLeft<eTile>();
    if(!agoraRoadTile(tl2)) return {};
    const auto tl3 = tl2->topLeft<eTile>();
    if(!agoraRoadTile(tl3)) return {};
    const auto tl4 = tl3->topLeft<eTile>();
    if(!agoraRoadTile(tl4)) return {};
    const auto tl5 = tl4->topLeft<eTile>();
    if(!agoraRoadTile(tl5)) return {};
    std::vector<eTile*> tiles;
    tiles.push_back(tl5);
    tiles.push_back(tl4);
    tiles.push_back(tl3);
    tiles.push_back(tl2);
    tiles.push_back(tl1);
    tiles.push_back(tile);
    const int iMin = tile->x() - 5;
    const int iMax = iMin + 6;
    const int jMin = tile->y() + 1;
    const int jMax = jMin + 2;
    for(int j = jMin; j < jMax; j++) {
        for(int i = iMin; i < iMax; i++) {
            if(!agoraSpaceTile(board, editorMode, i, j, cid, pid)) return {};
            tiles.push_back(board->tile(i, j));
        }
    }
    return tiles;
}

std::vector<eTile*> agoraBuildPlaceTR(GameBoard* board, bool editorMode,
                                      eTile* const tile,
                                      eCityId cid, ePlayerId pid) {
    if(!agoraRoadTile(tile)) return {};
    const auto tl1 = tile->topLeft<eTile>();
    if(!agoraRoadTile(tl1)) return {};
    const auto tl2 = tl1->topLeft<eTile>();
    if(!agoraRoadTile(tl2)) return {};
    const auto tl3 = tl2->topLeft<eTile>();
    if(!agoraRoadTile(tl3)) return {};
    const auto tl4 = tl3->topLeft<eTile>();
    if(!agoraRoadTile(tl4)) return {};
    const auto tl5 = tl4->topLeft<eTile>();
    if(!agoraRoadTile(tl5)) return {};
    std::vector<eTile*> tiles;
    tiles.push_back(tl5);
    tiles.push_back(tl4);
    tiles.push_back(tl3);
    tiles.push_back(tl2);
    tiles.push_back(tl1);
    tiles.push_back(tile);
    const int iMin = tile->x() - 5;
    const int iMax = iMin + 6;
    const int jMin = tile->y() - 3;
    const int jMax = jMin + 2;
    for(int j = jMax; j > jMin; j--) {
        for(int i = iMin; i < iMax; i++) {
            if(!agoraSpaceTile(board, editorMode, i, j, cid, pid)) return {};
            tiles.push_back(board->tile(i, j));
        }
    }
    return tiles;
}

std::vector<eTile*> agoraBuildPlaceIter(GameBoard* board, bool editorMode,
                                        eTile* const tile, bool grand,
                                        eAgoraOrientation& bt,
                                        eCityId cid, ePlayerId pid) {
    if(!tile) return {};
    {
        const int xMin = tile->x() - 2;
        const int xMax = xMin + 3;
        const int yMin = tile->y() + 2;
        const int yMax = yMin + 3;
        for(int x = xMin; x < xMax; x++) {
            for(int y = yMin; y < yMax; y++) {
                const auto t = board->tile(x, y);
                if(!t) continue;
                const auto r = agoraBuildPlaceBR(board, editorMode, t, cid, pid);
                if(r.empty()) continue;
                bt = eAgoraOrientation::bottomRight;
                if(grand) {
                    const auto rr = agoraBuildPlaceTL(board, editorMode, t, cid, pid);
                    if(rr.empty()) continue;
                    std::vector<eTile*> rrr;
                    rrr.reserve(r.size() + rr.size());
                    rrr.insert(rrr.end(), rr.begin(), rr.end());
                    rrr.insert(rrr.end(), r.begin(), r.end());
                    return rrr;
                }
                return r;
            }
        }
    }
    {
        const int xMin = tile->x();
        const int xMax = xMin + 3;
        const int yMin = tile->y() + 2;
        const int yMax = yMin + 3;
        for(int x = xMin; x < xMax; x++) {
            for(int y = yMin; y < yMax; y++) {
                const auto t = board->tile(x, y);
                if(!t) continue;
                const auto r = agoraBuildPlaceTL(board, editorMode, t, cid, pid);
                if(r.empty()) continue;
                if(grand) {
                    bt = eAgoraOrientation::bottomRight;
                    const auto rr = agoraBuildPlaceBR(board, editorMode, t, cid, pid);
                    if(rr.empty()) continue;
                    std::vector<eTile*> rrr;
                    rrr.reserve(r.size() + rr.size());
                    rrr.insert(rrr.end(), r.begin(), r.end());
                    rrr.insert(rrr.end(), rr.begin(), rr.end());
                    return rrr;
                } else {
                    bt = eAgoraOrientation::topLeft;
                }
                return r;
            }
        }
    }
    {
        const int xMin = tile->x() + 2;
        const int xMax = xMin + 3;
        const int yMin = tile->y() - 2;
        const int yMax = yMin + 3;
        for(int x = xMin; x < xMax; x++) {
            for(int y = yMin; y < yMax; y++) {
                const auto t = board->tile(x, y);
                if(!t) continue;
                const auto r = agoraBuildPlaceBL(board, editorMode, t, cid, pid);
                if(r.empty()) continue;
                bt = eAgoraOrientation::bottomLeft;
                if(grand) {
                    const auto rr = agoraBuildPlaceTR(board, editorMode, t, cid, pid);
                    if(rr.empty()) continue;
                    std::vector<eTile*> rrr;
                    rrr.reserve(r.size() + rr.size());
                    rrr.insert(rrr.end(), rr.begin(), rr.end());
                    rrr.insert(rrr.end(), r.begin(), r.end());
                    return rrr;
                }
                return r;
            }
        }
    }
    {
        const int xMin = tile->x() + 2;
        const int xMax = xMin + 3;
        const int yMin = tile->y();
        const int yMax = yMin + 3;
        for(int x = xMin; x < xMax; x++) {
            for(int y = yMin; y < yMax; y++) {
                const auto t = board->tile(x, y);
                if(!t) continue;
                const auto r = agoraBuildPlaceTR(board, editorMode, t, cid, pid);
                if(r.empty()) continue;
                if(grand) {
                    bt = eAgoraOrientation::bottomLeft;
                    const auto rr = agoraBuildPlaceBL(board, editorMode, t, cid, pid);
                    if(rr.empty()) continue;
                    std::vector<eTile*> rrr;
                    rrr.reserve(r.size() + rr.size());
                    rrr.insert(rrr.end(), r.begin(), r.end());
                    rrr.insert(rrr.end(), rr.begin(), rr.end());
                    return rrr;
                } else {
                    bt = eAgoraOrientation::topRight;
                }
                return r;
            }
        }
    }
    return {};
}

std::vector<eTile*> stampAgoraBuildPlace(GameBoard* board, bool editorMode,
                                         const eStampBuildCommand& cmd,
                                         int pressedTX, int pressedTY,
                                         eAgoraOrientation& bt,
                                         eCityId cid, ePlayerId pid) {
    const int tx = pressedTX + cmd.dx;
    const int ty = pressedTY + cmd.dy;
    const auto t = board->tile(tx, ty);
    std::vector<eTile*> result;

    if(cmd.agoraRect && cmd.agoraOrientation >= 0) {
        bt = static_cast<eAgoraOrientation>(cmd.agoraOrientation);
        const bool horizontal = bt == eAgoraOrientation::bottomLeft ||
                                bt == eAgoraOrientation::topRight;
        const int w = horizontal ? 6 : 3;
        const int h = horizontal ? 3 : 6;
        for(int x = tx; x < tx + w; x++) {
            for(int y = ty; y < ty + h; y++) {
                if(!agoraSpaceTile(board, editorMode, x, y, cid, pid)) return {};
                result.push_back(board->tile(x, y));
            }
        }
        return result;
    }

    if(!cmd.agoraRoads.empty()) {
        int bestScore = 0;
        const auto roadScore = [&](const std::vector<eTile*>& tiles) {
            int score = 0;
            for(const auto tile : tiles) {
                if(!tile || !tile->hasRoad()) continue;
                for(const auto& road : cmd.agoraRoads) {
                    if(tile->x() == pressedTX + road.first &&
                       tile->y() == pressedTY + road.second) {
                        score++;
                        break;
                    }
                }
            }
            return score;
        };
        const auto spaceScore = [&](const std::vector<eTile*>& tiles) {
            int score = 0;
            for(const auto tile : tiles) {
                if(!tile || tile->hasRoad()) continue;
                for(const auto& space : cmd.agoraSpaces) {
                    if(tile->x() == pressedTX + space.first &&
                       tile->y() == pressedTY + space.second) {
                        score++;
                        break;
                    }
                }
            }
            return score;
        };
        const auto testCandidate = [&](const std::vector<eTile*>& tiles,
                                       const eAgoraOrientation o) {
            const int score = 10*spaceScore(tiles) + roadScore(tiles);
            if(score > bestScore) {
                bestScore = score;
                bt = o;
                result = tiles;
            }
        };
        for(const auto& road : cmd.agoraRoads) {
            const auto rt = board->tile(pressedTX + road.first,
                                        pressedTY + road.second);
            testCandidate(agoraBuildPlaceBR(board, editorMode, rt, cid, pid),
                          eAgoraOrientation::bottomRight);
            testCandidate(agoraBuildPlaceTL(board, editorMode, rt, cid, pid),
                          eAgoraOrientation::topLeft);
            testCandidate(agoraBuildPlaceBL(board, editorMode, rt, cid, pid),
                          eAgoraOrientation::bottomLeft);
            testCandidate(agoraBuildPlaceTR(board, editorMode, rt, cid, pid),
                          eAgoraOrientation::topRight);
        }
        return result;
    }

    if(cmd.agoraOrientation >= 0) {
        bt = static_cast<eAgoraOrientation>(cmd.agoraOrientation);
        switch(bt) {
        case eAgoraOrientation::bottomRight:
            return agoraBuildPlaceBR(board, editorMode, t, cid, pid);
        case eAgoraOrientation::topLeft:
            return agoraBuildPlaceTL(board, editorMode, t, cid, pid);
        case eAgoraOrientation::bottomLeft:
            return agoraBuildPlaceBL(board, editorMode, t, cid, pid);
        case eAgoraOrientation::topRight:
            return agoraBuildPlaceTR(board, editorMode, t, cid, pid);
        }
    }

    return agoraBuildPlaceIter(board, editorMode, t, false, bt, cid, pid);
}
