#ifndef TraderAction_H
#define TraderAction_H

#include <map>

#include "eactionwithcomeback.h"
#include "buildings/trade-post.h"
#include "ewalkablehelpers.h"

class eSaveArchive;

class TraderAction : public eActionWithComeback {
    friend class eTA_tradeFail;
    friend class eTA_tradeFinish;
    friend class eTA_goToTradePostFinish;
public:
    TraderAction(eCharacter* const c);

    bool decide() override;

    void setTradePost(TradePost* const tp);
    void setUnpackBuilding(eBuilding* const b);

    void setWalkable(const stdsptr<eWalkableObject>& w);

    const std::map<eResourceType, int>& bought() const { return mBought; }
    const std::map<eResourceType, int>& sold() const { return mSold; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    void goToTradePost();
    void trade();
    void tradeIncrement();

    stdsptr<eWalkableObject> mWalkable =
            eWalkableObject::sCreateDefault();

    int mCash = 1000;
    int mItems = 1000;
    stdptr<TradePost> mTradePost;
    stdptr<eBuilding> mUnpackBuilding;
    bool mAtTradePost = false;
    bool mFinishedTrade = false;
    bool mNotFound = false;

    std::map<eResourceType, int> mBought;
    std::map<eResourceType, int> mSold;
};

class eTA_tradeFail : public eCharActFunc {
public:
    eTA_tradeFail(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::TA_tradeFail) {}
    eTA_tradeFail(eGameBoard& board, TraderAction* const t) :
        eCharActFunc(board, eCharActFuncType::TA_tradeFail),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->goBack(t->mWalkable);
    }
protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<TraderAction> mTptr;
};

class eTA_tradeFinish : public eCharActFunc {
public:
    eTA_tradeFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::TA_tradeFinish) {}
    eTA_tradeFinish(eGameBoard& board, TraderAction* const t) :
        eCharActFunc(board, eCharActFuncType::TA_tradeFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->tradeIncrement();
    }
protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<TraderAction> mTptr;
};

class eTA_goToTradePostFinish : public eCharActFunc {
public:
    eTA_goToTradePostFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::TA_goToTradePostFinish) {}
    eTA_goToTradePostFinish(eGameBoard& board, TraderAction* const t) :
        eCharActFunc(board, eCharActFuncType::TA_goToTradePostFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        if(!t->mTradePost) return;
        t->mAtTradePost = true;
        const auto tt = t->mTradePost->tpType();
        if(tt == eTradePostType::pier) {
            const auto o = t->mTradePost->orientation();
            eOrientation oo;
            switch(o) {
            case eDiagonalOrientation::bottomLeft:
            case eDiagonalOrientation::topRight:
                oo = eOrientation::topLeft;
                break;
            default:
            case eDiagonalOrientation::bottomRight:
            case eDiagonalOrientation::topLeft:
                oo = eOrientation::topRight;
                break;
            }
            const auto c = t->character();
            c->setOrientation(oo);
        }
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<TraderAction> mTptr;
};

#endif // TraderAction_H
