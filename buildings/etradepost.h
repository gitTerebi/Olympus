#ifndef ETRADEPOST_H
#define ETRADEPOST_H

#include "ewarehousebase.h"

#include "engine/e-worldcity.h"
#include "ewalkablehelpers.h"

class eSaveArchive;

enum class eTradePostType {
    post, pier
};

class eTradePost : public eWarehouseBase {
public:
    eTradePost(eGameBoard& board, eWorldCity& city,
               const eCityId cid,
               const eTradePostType type = eTradePostType::post);
    ~eTradePost();

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    void erase() override;

    void setOrders(const eResourceType imports,
                   const eResourceType exports,
                   const eResourceType empty = eResourceType::none,
                   const eResourceType cartGet = eResourceType::none,
                   const eResourceType cartAccept = eResourceType::none);

    void getOrders(eResourceType& imports,
                   eResourceType& exports,
                   eResourceType& empty,
                   eResourceType& cartGet,
                   eResourceType& cartAccept) const;

    eWorldCity& city() const { return mCity; }

    void spawnTrader();

    bool playerTwoWay() const;

    int buy(const int cash);
    int sell(const int items);

    int buy(const int cash, std::map<eResourceType, int>& bought);
    int sell(const int items, std::map<eResourceType, int>& sold);

    void setWalkable(const stdsptr<eWalkableObject>& w);
    void setUnpackBuilding(eBuilding* const b);
    void setOrientation(const eDiagonalOrientation o);
    eDiagonalOrientation orientation() const;
    eTradePostType tpType() const { return mType; }

    using eCharacterCreator =
        std::function<stdsptr<eCharacter>(eTile*, eGameBoard&)>;
    void setCharacterCreator(const eCharacterCreator& c);

    eBuilding* unpackBuilding() const { return mUnpackBuilding; }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    bool trades() const;
private:
    void serialize(eSaveArchive& ar);
    eTile* entryPoint() const;

    eWorldCity& mCity;
    const eTradePostType mType;
    eResourceType mImports = eResourceType::none;
    eResourceType mExports = eResourceType::none;
    eResourceType mCartEmpty = eResourceType::none;
    eResourceType mCartGet = eResourceType::none;
    eResourceType mCartAccept = eResourceType::none;
    eDiagonalOrientation mO = eDiagonalOrientation::topLeft;

    eCharacterCreator mCharGen;
    stdsptr<eWalkableObject> mWalkable = eWalkableObject::sCreateDefault();
    eBuilding* mUnpackBuilding = this;

    int mRouteTimer = 0;
};

#endif // ETRADEPOST_H
