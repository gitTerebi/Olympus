#include "ewolfaction.h"

#include "erand.h"
#include "enumbers.h"
#include "emovetoaction.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

#include "engine/epathfinder.h"
#include "characters/actions/walkable/ewalkableobject.h"
#include "buildings/ebuilding.h"
#include "audio/sounds.h"

eWolfAction::eWolfAction(eCharacter *const c, const int spawnerX, const int spawnerY) : eAnimalAction(c, spawnerX, spawnerY,
                                                                                                      eWalkableObject::sCreateDefault(),
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
    eAnimalAction::increment(by);
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
    const int wait = eNumbers::sWolfHuntWait;
    if (wait <= 0)
        return eAnimalAction::decide();
    const bool hunt = (eRand::rand() % wait) == 0;
    if (hunt)
    {
        mHunting = true;
        findPrey();
        return true;
    }
    return eAnimalAction::decide();
}

void eWolfAction::serializeFields(eSaveArchive& ar)
{
    eAnimalAction::serializeFields(ar);
    ar.field("hunting", mHunting);
    ar.field("stage", mStage);
    ar.buildingAsField("wallTarget", &board(), mWallTarget);
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
}

void eWolfAction::goBack()
{
    mHunting = false;
    mStage = eWolfActionStage::goingBack;
    mWallTarget.clear();

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
    a->setMaxFindDistance(2 * eNumbers::sWolfHuntDistance);
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
        const auto wallWalkable = std::bind(&eWalkableObject::walkable,
                                            eWalkableObject::sCreateWall().get(),
                                            std::placeholders::_1);
        const auto pf = std::make_shared<ePathFinder>(wallWalkable, hha);
        const auto &board = this->board();
        const SDL_Rect r{0, 0, board.width(), board.height()};
        if (!pf->findPath(r, ct, eNumbers::sWolfHuntDistance, false, 1, 1)) {
            goBack();
            return;
        }
        std::vector<eTile *> path;
        pf->extractPath(path, const_cast<eGameBoard &>(board));
        eBuilding *wall = nullptr;
        for (const auto &tile : path) {
            const auto ub = tile->underBuilding();
            if (ub && ub->type() == eBuildingType::wall) { wall = ub; break; }
        }
        if (wall) attackWall(wall);
        else goBack();
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(eNumbers::sWolfHuntDistance);
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
