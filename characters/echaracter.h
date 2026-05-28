#ifndef ECHARACTER_H
#define ECHARACTER_H

#include "pointers/eobject.h"
#include "pointers/estdpointer.h"

#include "eoverlay.h"
#include "engine/eorientation.h"
#include "engine/etile.h"
#include "etexture.h"
#include "echaracterbase.h"
#include "engine/eprovide.h"
#include "characters/actions/character-action-function.h"

#include "actions/earcheraction.h"
#include "actions/edieaction.h"
#include "fileIO/esavearchive.h"

class GameBoard;
class eCharacterAction;
class eSaveArchive;

struct ePausedAction {
    eCharacterActionType fAt;
    stdsptr<eCharacterAction> fA;
    eOrientation fO;
};

class eCharacter : public eObject, public eCharacterBase {
public:
    eCharacter(GameBoard& board, const eCharacterType type);
    virtual ~eCharacter();

    virtual std::shared_ptr<eTexture> getTexture(const eTileSize size) const = 0;
    virtual eOverlay getSecondaryTexture(const eTileSize size) const {
        (void)size;
        return eOverlay();
    }
    virtual bool canFight(eCharacter* const c);

    virtual void provideToBuilding(eBuilding* const b);

    virtual void beingKilled() {}

    virtual void incTime(const int by);

    void setCityId(const eCityId i) override;

    void fight(eCharacter* const c);
    void setPlayFightSound(const bool p);
    void kill();
    void killWithCorpse();

    double x() const { return mX; }
    double y() const { return mY; }
    double absX() const;
    double absY() const;

    eTile* tile() const { return mTile; }
    int time() const { return mTime; }
    static const int sTextureTimeDivisor = 20;
    int textureTime() const { return time()/sTextureTimeDivisor; }
    eOrientation orientation() const { return mOrientation; }
    eOrientation rotatedOrientation() const;

    void changeTile(eTile* const t, const bool prepend = false);
    void setX(const double x) { mX = x; }
    void setY(const double y) { mY = y; };
    void setOrientation(const eOrientation o);

    void setAction(const stdsptr<eCharacterAction>& a);
    eCharacterAction* action() const { return mAction.get(); }

    virtual void setActionType(const eCharacterActionType t);
    int actionStartTime() const { return mActionStartTime; }

    bool hasSecondaryTexture() const { return mHasSecondaryTexture; }
    void setHasSecondaryTexture(const bool st);

    int provideCount() const { return mProvideCount; }
    void setProvide(const eProvide p, const int n);

    bool visible() const { return mVisible; }
    void setVisible(const bool v) { mVisible = v; }

    bool takeDamage(const double a);
    virtual bool takeDamage(const double a, eCharacter* const attacker);
    bool takeMeleeDamage(const double a, eCharacter* const attacker);

    static stdsptr<eCharacter> sCreate(
            const eCharacterType t, GameBoard& board);

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    void pauseAction();
    void resumeAction();
    void clearPausedActions();

    ePlayerId playerId() const;
    eTeamId teamId() const;
    int seedId() const { return mSeedId; }
protected:
    std::shared_ptr<eTexture> getTexture(
            const eTextureCollection* const coll,
            const bool wrap, const bool reverse,
            const bool disappear = false) const;
    void serializeFields(eSaveArchive& ar) override;
    GameBoard& ownerBoard() const;
private:
    void restorePausedAction(const ePausedAction& p);

    std::vector<ePausedAction> mPausedActions;

    int mIOID = -1;

    int mSeedId;

    bool mVisible = true;

    eProvide mProvide = eProvide::none;
    int mProvideCount = 0;

    eTile* mTile = nullptr;
    eOrientation mOrientation{eOrientation::top};
    double mX = 0.5;
    double mY = 0.5;

    bool mPlayFightSound = false;
    int mSoundPlayTime = 0;
    int mTime = 0;
    bool mHasSecondaryTexture = false;

    stdsptr<eCharacterAction> mAction;
    int mActionStartTime{0};
};

class eChar_fightFinish : public eCharActFunc {
public:
    eChar_fightFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::Char_fightFinish) {}
    eChar_fightFinish(GameBoard& board, eCharacter* const t) :
        eCharActFunc(board, eCharActFuncType::Char_fightFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        if(t->dead()) {
            const auto d = e::make_shared<eDieAction>(t);
            t->setAction(d);
        } else {
            t->resumeAction();
        }
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("character", &board(), mTptr);
    }
private:
    stdptr<eCharacter> mTptr;
};

class eChar_killWithCorpseFinish : public eCharActFunc {
public:
    eChar_killWithCorpseFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::Char_killWithCorpseFinish) {}
    eChar_killWithCorpseFinish(GameBoard& board, eCharacter* const t,
                               const bool withCorpse = false) :
        eCharActFunc(board, eCharActFuncType::Char_killWithCorpseFinish),
        mWithCorpse(withCorpse), mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        if(mWithCorpse && !t->dead()) t->killWithCorpse();
        else t->kill();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.field("withCorpse", mWithCorpse);
        ar.characterField("character", &board(), mTptr);
    }
private:
    bool mWithCorpse;
    stdptr<eCharacter> mTptr;
};

#endif // ECHARACTER_H
