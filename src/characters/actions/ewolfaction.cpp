#include "ewolfaction.h"

#include "rand.h"
#include "numbers.h"
#include "emovetoaction.h"
#include "engine/game-board.h"
#include "fileIO/save-archive.h"

#include "engine/epathfinder.h"
#include "characters/actions/walkable/walkable-object.h"
#include "buildings/ebuilding.h"
#include "audio/sounds.h"
#include "characters/echaracter.h"

#include <cstdlib>

eWolfAction::eWolfAction(eCharacter *const c, const int spawnerX, const int spawnerY) : AnimalAction(c, spawnerX, spawnerY,
                                                                                                      WalkableObject::sCreateDefault(),
                                                                                                      eCharActionType::wolfAction) {}

eWolfAction::eWolfAction(eCharacter *const c) : eWolfAction(c, 0, 0) {}

bool killedPrey(eTile *const tile)
{
    const auto cs = tile->characters();
    for (const auto &c : cs)
    {
        const auto t = c->type();
        if (t == eCharacterType::sheep ||
            t == eCharacterType::goat ||
            t == eCharacterType::cattle1 ||
            t == eCharacterType::cattle2 ||
            t == eCharacterType::cattle3)
        {
            if (c->dead())
                return true;
        }
    }
    return false;
}

void eWolfAction::increment(const int by)
{
    const auto c = character();
    if (mWallTarget) {
        const bool dead = mWallTarget->takeDamage(by * c->attack());

        if (dead) {
            eSounds::playCollapseSound();
            mWallTarget.clear();
            findPrey();
        }
        return;
    }
    const auto t = c->tile();
    if (t && killedPrey(t))
    {
        goBack();
        return;
    }
    AnimalAction::increment(by);
}

static eBuilding* sAdjacentWall(eTile* const t) {
    if (!t) return nullptr;
    const auto check = [](eTile* const n) -> eBuilding* {
        if (!n) return nullptr;
        const auto ub = n->underBuilding();
        if (ub && ub->type() == eBuildingType::wall) return ub;
        return nullptr;
    };
    if (auto* w = check(t->topLeft<eTile>())) return w;
    if (auto* w = check(t->topRight<eTile>())) return w;
    if (auto* w = check(t->bottomLeft<eTile>())) return w;
    if (auto* w = check(t->bottomRight<eTile>())) return w;
    return nullptr;
}

bool eWolfAction::decide()
{
    if(mRetaliationTarget) {
        if(canAttackRetaliationTarget()) {
            character()->fight(mRetaliationTarget.get());
        } else {
            moveToRetaliationTarget();
        }
        return true;
    }
    if (mHunting)
    {
        findPrey();
        return true;
    }
    const auto c = character();
    if (auto* wall = sAdjacentWall(c->tile())) {
        attackWall(wall);
        return true;
    }
    const int wait = Numbers::sWolfHuntWait;
    if (wait <= 0)
        return AnimalAction::decide();
    const bool hunt = (Rand::rand() % wait) == 0;
    if (hunt)
    {
        mHunting = true;
        findPrey();
        return true;
    }
    return AnimalAction::decide();
}

void eWolfAction::serializeFields(SaveArchive& ar)
{
    AnimalAction::serializeFields(ar);
    ar.field("hunting", mHunting);
    ar.field("stage", mStage);
    ar.buildingAsField("wallTarget", &board(), mWallTarget);
    ar.characterField("retaliationTarget", &board(), mRetaliationTarget);
}

void eWolfAction::resumeFromSavedState()
{
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eWolfActionStage::idle:
        eComplexAction::resumeFromSavedState();
        break;
    case eWolfActionStage::hunting:
        findPrey();
        break;
    case eWolfActionStage::goingBack:
        goBack();
        break;
    case eWolfActionStage::attackingWall:
        if(mWallTarget) {
            character()->setActionType(eCharacterActionType::fight);
        } else {
            findPrey();
        }
        break;
    }
    if(mRetaliationTarget) moveToRetaliationTarget();
}

void eWolfAction::goBack()
{
    mHunting = false;
    mStage = eWolfActionStage::goingBack;
    mWallTarget.clear();
    mRetaliationTarget.clear();

    const auto c = character();

    const auto &board = eWolfAction::board();
    const auto tile = board.tile(mSpawnerX, mSpawnerY);
    if (!tile)
    {
        c->kill();
        return;
    }

    const stdptr<eCharacter> cptr(c);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFoundAction([cptr, c]()
                      {
        if(!cptr) return;
        c->setActionType(eCharacterActionType::walk); });
    const auto findFailFunc = [cptr, c]()
    {
        if (!cptr)
            return;
        c->kill();
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(2 * Numbers::sWolfHuntDistance);
    a->start(tile);
    setCurrentAction(a);
}

void eWolfAction::findPrey()
{
    mHunting = true;
    mStage = eWolfActionStage::hunting;
    const auto c = character();

    const stdptr<eCharacter> cptr(c);
    const stdptr<eWolfAction> wolfActionRef(this);

    const auto hha = [](eTileBase *const tile)
    {
        return tile->hasCharacter([](const eCharacterBase &c)
                                  {
            const auto type = c.type();
            return type == eCharacterType::sheep ||
                   type == eCharacterType::goat ||
                   type == eCharacterType::cattle1 ||
                   type == eCharacterType::cattle2 ||
                   type == eCharacterType::cattle3; });
    };

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFoundAction([cptr, c]()
                      {
        if(!cptr) return;
        c->setActionType(eCharacterActionType::walk); });
    const auto findFailFunc = [wolfActionRef, this, hha]()
    {
        if (!wolfActionRef)
            return;
        const auto c = character();
        const auto ct = c->tile();
        if (!ct)
        {
            goBack();
            return;
        }
        const auto wallWalkableObject = WalkableObject::sCreateWall();
        const auto wallWalkable = [wallWalkableObject](eTileBase* const t) {
            return wallWalkableObject->walkable(t);
        };
        const auto pf = std::make_shared<ePathFinder>(wallWalkable, hha);
        const auto &board = this->board();
        const SDL_Rect r{0, 0, board.width(), board.height()};
        if (!pf->findPath(r, ct, Numbers::sWolfHuntDistance, false, 1, 1)) {
            goBack();
            return;
        }
        std::vector<eTile *> path;
        pf->extractPath(path, const_cast<GameBoard &>(board));
        eBuilding *wall = nullptr;
        for (const auto &tile : path) {
            const auto ub = tile->underBuilding();
            if (ub && ub->type() == eBuildingType::wall) { wall = ub; break; }
        }
        if (wall) attackWall(wall);
        else goBack();
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(Numbers::sWolfHuntDistance);
    a->start(hha);
    setCurrentAction(a);
}

void eWolfAction::attackWall(eBuilding *const wall)
{
    setCurrentAction(nullptr);
    mWallTarget = wall;
    mStage = eWolfActionStage::attackingWall;
    character()->setActionType(eCharacterActionType::fight);
}

void eWolfAction::retaliate(eCharacter* const attacker)
{
    if(!attacker || attacker->dead()) return;
    mHunting = false;
    mWallTarget.clear();
    mRetaliationTarget = attacker;
    if(canAttackRetaliationTarget()) {
        character()->fight(attacker);
    } else {
        moveToRetaliationTarget();
    }
}

bool eWolfAction::canAttackRetaliationTarget() const
{
    if(!mRetaliationTarget || mRetaliationTarget->dead()) return false;
    const auto c = character();
    const auto ct = c->tile();
    const auto tt = mRetaliationTarget->tile();
    if(!ct || !tt) return false;
    return abs(ct->x() - tt->x()) <= 1 &&
           abs(ct->y() - tt->y()) <= 1;
}

void eWolfAction::moveToRetaliationTarget()
{
    if(!mRetaliationTarget || mRetaliationTarget->dead()) {
        mRetaliationTarget.clear();
        goBack();
        return;
    }

    const auto c = character();
    const stdptr<eCharacter> cptr(c);
    const stdptr<eWolfAction> actionPtr(this);
    const stdptr<eCharacter> targetPtr(mRetaliationTarget.get());

    const auto targetTile = mRetaliationTarget->tile();
    const int tx = targetTile ? targetTile->x() : 0;
    const int ty = targetTile ? targetTile->y() : 0;
    const auto final = [targetPtr, tx, ty](eThreadTile* const t) {
        if(targetPtr && targetPtr->tile()) {
            const auto tt = targetPtr->tile();
            return abs(t->x() - tt->x()) <= 1 &&
                   abs(t->y() - tt->y()) <= 1;
        }
        return abs(t->x() - tx) <= 1 &&
               abs(t->y() - ty) <= 1;
    };

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFoundAction([cptr]() {
        if(!cptr) return;
        cptr->setActionType(eCharacterActionType::walk);
    });
    a->setFindFailAction([actionPtr]() {
        if(!actionPtr) return;
        actionPtr->goBack();
    });
    a->setMaxFindDistance(Numbers::sWolfHuntDistance * 2);
    a->start(final);
    setCurrentAction(a);
}
