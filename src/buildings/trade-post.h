#ifndef TRADE_POST_H
#define TRADE_POST_H

#include "warehouse-base.h"

#include "engine/e-worldcity.h"
#include "ewalkablehelpers.h"

class eSaveArchive;

enum class eTradePostType {
    post, pier
};

class TradePost : public WarehouseBase {
public:
    TradePost(GameBoard& board, eWorldCity& city,
               const eCityId cid,
               const eTradePostType type = eTradePostType::post);
    ~TradePost();

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    eTextureSpace getTextureSpace(const int tx, const int ty,
                                  const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    void erase() override;

    void setOrders(const eResourceType imports,
                   const eResourceType exports,
                   const eResourceType empty = eResourceType::none,
                   const eResourceType cartGet = eResourceType::none,
                   const eResourceType cartAccept = eResourceType::none,
                   const eResourceType cartDontAccept = eResourceType::none);

    void getOrders(eResourceType& imports,
                   eResourceType& exports,
                   eResourceType& empty,
                   eResourceType& cartGet,
                   eResourceType& cartAccept,
                   eResourceType& cartDontAccept) const;

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
        std::function<stdsptr<eCharacter>(eTile*, GameBoard&)>;
    void setCharacterCreator(const eCharacterCreator& c);

    eBuilding* unpackBuilding() const { return mUnpackBuilding; }

    bool trades() const;
    bool importsResource(const eResourceType res) const override
    { return static_cast<bool>((mImports | mExports) & res); }
protected:
    void serializeFields(eSaveArchive& ar) override;
    // trade post pushes out imported goods only, never exports it holds
    bool pushAllows(const eResourceType res) const override
    { return static_cast<bool>(mImports & res); }
    // vanilla: 1 retrieve (get) + 1 distribute (deliver)
    int maxGetCarts() const override { return 1; }
    int maxDeliverCarts() const override { return 1; }
private:
    eTile* entryPoint() const;

    eWorldCity& mCity;
    const eTradePostType mType;
    eResourceType mImports = eResourceType::none;
    eResourceType mExports = eResourceType::none;
    eResourceType mCartEmpty = eResourceType::none;
    eResourceType mCartGet = eResourceType::none;
    eResourceType mCartAccept = eResourceType::none;
    eResourceType mCartDontAccept = eResourceType::none;
    eDiagonalOrientation mO = eDiagonalOrientation::topLeft;

    eCharacterCreator mCharGen;
    stdsptr<eWalkableObject> mWalkable = eWalkableObject::sCreateDefault();
    eBuilding* mUnpackBuilding = this;

    int mRouteTimer = 0;
};

#endif // TRADE_POST_H
