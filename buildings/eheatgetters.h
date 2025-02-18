#ifndef EHEATGETTERS_H
#define EHEATGETTERS_H

#include "buildings/ebuilding.h"
#include "engine/boardData/eheatmap.h"

enum class eGodType;

namespace eHeatGetters {
    using eHeatGetter = std::function<eHeat(eBuildingType)>;

    eHeat appeal(const eBuildingType type);
    eHeat housing(const eBuildingType type);
    eHeat culture(const eBuildingType type);
    eHeat industry(const eBuildingType type);
    eHeat farming(const eBuildingType type);
    eHeat storage(const eBuildingType type);

    eHeatGetter godLeaning(const eGodType gt);

    eHeat empty(const eBuildingType type);
    eHeat any(const eBuildingType type);

    eHeat fertile(eTileBase* const tile);
    eHeat notFertile(eTileBase* const tile);

    template <int TDist, int TAmpl, int TInflRange>
    eHeat distanceFromNotBuildable(eTileBase* const tile) {
        int minDist = 100000;
        const int tx = tile->x();
        const int ty = tile->y();
        for(int x = tx - TDist; x <= tx + TDist; x++) {
            for(int y = ty - TDist; y <= ty + TDist; y++) {
                const double x2 = std::pow(x - tx, 2);
                const double y2 = std::pow(y - ty, 2);
                const double distF = sqrt(x2 + y2);
                const int dist = std::round(distF);
                if(dist >= minDist) continue;
                const auto t = tile->tileAbs(x, y);
                if(!t) continue;
                const auto type = t->underBuildingType();
                const auto terr = t->terrain();
                const bool bterr = static_cast<bool>(terr & eTerrain::buildableAfterClear);
                if(bterr && (type == eBuildingType::none ||
                   type == eBuildingType::road)) continue;
                minDist = dist;
                if(minDist < TDist) return {0, 0};
            }
        }
        if(minDist == TDist) {
            return eHeat{TAmpl, TInflRange};
        }
        return eHeat{0, 0};
    }

    template <int TDist, int TAmpl, int TInflRange>
    eHeat distanceFromBuilding(eTileBase* const tile) {
        int minDist = 100000;
        const int tx = tile->x();
        const int ty = tile->y();
        for(int x = tx - TDist; x <= tx + TDist; x++) {
            for(int y = ty - TDist; y <= ty + TDist; y++) {
                const double x2 = std::pow(x - tx, 2);
                const double y2 = std::pow(y - ty, 2);
                const double distF = sqrt(x2 + y2);
                const int dist = std::round(distF);
                if(dist >= minDist) continue;
                const auto t = tile->tileAbs(x, y);
                if(!t) continue;
                const auto type = t->underBuildingType();
                if(type == eBuildingType::none ||
                   type == eBuildingType::road) continue;
                minDist = dist;
                if(minDist < TDist) return {0, 0};
            }
        }
        if(minDist == TDist) {
            return eHeat{TAmpl, TInflRange};
        }
        return eHeat{0, 0};
    }

    template <int TDist, int TAmpl, int TInflRange>
    eHeat distanceFromFertile(eTileBase* const tile) {
        int minDist = 100000;
        const int tx = tile->x();
        const int ty = tile->y();
        for(int x = tx - TDist; x <= tx + TDist; x++) {
            for(int y = ty - TDist; y <= ty + TDist; y++) {
                const double x2 = std::pow(x - tx, 2);
                const double y2 = std::pow(y - ty, 2);
                const double distF = sqrt(x2 + y2);
                const int dist = std::round(distF);
                if(dist >= minDist) continue;
                const auto t = tile->tileAbs(x, y);
                if(!t) continue;
                const auto terr = t->terrain();
                if(!static_cast<bool>(terr & eTerrain::fertile)) continue;
                minDist = dist;
                if(minDist < TDist) return {0, 0};
            }
        }
        if(minDist == TDist) {
            return eHeat{TAmpl, TInflRange};
        }
        return eHeat{0, 0};
    }

    template <int TDist, int TAmpl, int TInflRange>
    eHeat distanceFromPrey(eTileBase* const tile) {
        int minDist = 100000;
        const int tx = tile->x();
        const int ty = tile->y();
        for(int x = tx - TDist; x <= tx + TDist; x++) {
            for(int y = ty - TDist; y <= ty + TDist; y++) {
                const double x2 = std::pow(x - tx, 2);
                const double y2 = std::pow(y - ty, 2);
                const double distF = sqrt(x2 + y2);
                const int dist = std::round(distF);
                if(dist >= minDist) continue;
                const auto t = tile->tileAbs(x, y);
                if(!t) continue;
                const bool p = t->hasPrey();
                if(!p) continue;
                minDist = dist;
                if(minDist < TDist) return {0, 0};
            }
        }
        if(minDist == TDist) {
            return eHeat{TAmpl, TInflRange};
        }
        return eHeat{0, 0};
    }
}

#endif // EHEATGETTERS_H
