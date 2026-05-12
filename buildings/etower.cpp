#include "etower.h"

#include "textures/egametextures.h"

#include "characters/actions/earcheraction.h"
#include "vec2.h"
#include "missiles/earrowmissile.h"
#include "characters/efightingcharacter.h"
#include "characters/actions/efightingaction.h"
#include "audio/esounds.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

eTower::eTower(eGameBoard &board, const eCityId cid) : eEmployingBuilding(board, eBuildingType::tower, 2, 2, 15, cid)
{
    eGameTextures::loadGatehouseAndTower();
    setHP(eNumbers::sTowerHP);
    if (atlantean())
    {
        eGameTextures::loadPoseidonTowerArcher();
    }
    else
    {
        eGameTextures::loadArcher();
    }
}

eTower::~eTower()
{
    if (mArcher)
        mArcher->kill();
    if (!mDeleteArchers)
        return;
    const auto &tiles = tilesUnder();
    for (const auto tile : tiles)
    {
        if (!tile)
            continue;
        const auto chars = tile->characters();
        for (const auto &c : chars)
        {
            const auto type = c->type();
            if (type == eCharacterType::archer)
                c->kill();
        }
    }
}

eTowerEmploymentState eTower::employmentState(const bool mManTowers, const bool hasPalace) const
{
    if (!hasPalace)
        return eTowerEmploymentState::noPalace;
    if (!getBoard().hasRoadToPalace(centerTile()))
        return eTowerEmploymentState::noRoad;
    if (!mManTowers)
        return eTowerEmploymentState::shutdown;
    return eTowerEmploymentState::available;
}

std::shared_ptr<eTexture>
eTower::getTexture(const eTileSize size) const
{
    const int sizeId = static_cast<int>(size);
    const auto &texs = eGameTextures::buildings();
    return texs[sizeId].fTower;
}

std::vector<eOverlay>
eTower::getOverlays(const eTileSize size) const
{
    const int sizeId = static_cast<int>(size);
    const auto &texs = eGameTextures::characters()[sizeId];
    const eArcherTextures *aTexs;
    if (atlantean())
    {
        aTexs = &texs.fPoseidonTowerArcher;
    }
    else
    {
        aTexs = &texs.fArcher;
    }
    if (mAttack)
    {
        const int oid = static_cast<int>(mAttackOrientation);
        const auto &coll = &aTexs->fFight[oid];
        const int t = time() / 20;
        const int texId = t % coll->size();
        eOverlay o;
        o.fTex = coll->getTexture(texId);
        o.fX = -3.5;
        o.fY = -5.5;
        return {o};
    }
    else
    {
        const auto &colls = aTexs->fPatrol;
        const int tt = textureTime();
        const int idid = tt / 50 % 8;
        const int ids[8] = {5, 0, 3, 7, 4, 2, 6, 1};
        const int id = ids[idid];
        const auto &coll = colls[id];
        eOverlay o;
        o.fTex = coll.getTexture(tt % coll.size());
        o.fX = -3.5;
        o.fY = -5.5;
        return {o};
    }
}

void eTower::timeChanged(const int by)
{
    if (enabled())
    {
        // Only spawn archers when fully employed (15/15)
        if (employed() >= maxEmployees())
        {
            if (!mArcher)
            {
                mSpawnTime += by;
                if (mSpawnTime > eNumbers::sTowerSpawnPeriod)
                {
                    spawn();
                    mSpawnTime = 0;
                }
            }
        }
        else if (mArcher)
        {
            // Kill archer if not fully staffed
            mArcher->kill();
        }

        const int rangeAttackCheck = 500;
        const int missileCheck = 200;
        const int range = eNumbers::sTowerRange;
        const double attack = eNumbers::sTowerAttack;

        const auto ct = centerTile();
        const int tx = ct->x();
        const int ty = ct->y();
        const vec2d cpos{1. * tx, 1. * ty};
        const auto tid = teamId();
        auto &brd = getBoard();

        if (mAttack)
        {
            if (range > 0 && mAttackTarget)
            {
                mMissile += by;
                if (mMissile > missileCheck)
                {
                    mMissile = mMissile - missileCheck;
                    const auto tt = mAttackTarget->tile();
                    const int ttx = tt->x();
                    const int tty = tt->y();
                    eMissile::sCreate<eArrowMissile>(brd, tx, ty, 3.5,
                                                     ttx, tty, 0.5, 2);
                    auto &board = getBoard();
                    board.ifVisible(centerTile(), [&]()
                                    { eSounds::playAttackSound(eCharacterType::archer); });
                }
            }
            mAttackTime += by;
            bool finishAttack = !mAttackTarget ||
                                mAttackTarget->dead() ||
                                mAttackTime > 1000;
            if (mAttackTarget && !mAttackTarget->dead())
            {
                const double att = by * attack;
                const bool d = mAttackTarget->defend(att);
                if (d)
                    finishAttack = true;
            }
            if (finishAttack)
            {
                mAttack = false;
                mAttackTarget = nullptr;
                mAttackTime = 0;
                mRangeAttack = rangeAttackCheck;
            }
            else
            {
                return;
            }
        }

        mRangeAttack += by;
        if (mRangeAttack > rangeAttackCheck)
        {
            mRangeAttack = mRangeAttack - rangeAttackCheck;
            for (int i = -range; i <= range; i++)
            {
                for (int j = -range; j <= range; j++)
                {
                    const auto t = brd.tile(tx + i, ty + j);
                    if (!t)
                        continue;
                    const auto &chars = t->characters();
                    for (const auto &cc : chars)
                    {
                        if (!cc->isFighter())
                            continue;
                        const auto cctid = cc->teamId();
                        if (!eTeamIdHelpers::isEnemy(cctid, tid))
                            continue;
                        if (cc->dead())
                            continue;
                        const vec2d ccpos{cc->absX(), cc->absY()};
                        const vec2d posdif = ccpos - cpos;
                        mAttackTarget = cc;
                        mAttack = true;
                        mAttackTime = 0;
                        const double angle = posdif.angle();
                        const auto o = sAngleOrientation(angle);
                        mAttackOrientation = o;

                        const auto tt = cc->tile();
                        const int ttx = tt->x();
                        const int tty = tt->y();
                        eFightingAction::sSignalBeingAttack(cc.get(), ttx, tty, brd);

                        return;
                    }
                }
            }
        }
    }
    else
    {
        if (mArcher)
            mArcher->kill();
    }
    eEmployingBuilding::timeChanged(by);
}

bool eTower::enabled() const
{
    if (!eEmployingBuilding::enabled())
        return false;
    // Towers need adjacent road and road connecting to palace to function
    return accessToRoad() && getBoard().hasRoadToPalace(centerTile());
}

void eTower::setEmployed(const int e)
{
    const auto cid = cityId();
    const auto &board = getBoard();
    const bool militaryDisabled = board.isShutDown(cid, eBuildingType::tower);
    const bool hasPalace = board.hasPalace(cid);
    const auto state = employmentState(!militaryDisabled, hasPalace);
    if (state == eTowerEmploymentState::available)
    {
        eEmployingBuilding::setEmployed(e);
    }
    else
    {
        eEmployingBuilding::setEmployed(0);
    }
}

void eTower::read(eReadStream &src)
{
    eEmployingBuilding::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eTower::write(eWriteStream &dst) const
{
    eEmployingBuilding::write(dst);
    eSaveArchive ar(dst);
    const_cast<eTower *>(this)->serialize(ar);
}

void eTower::serialize(eSaveArchive &ar)
{
    ar.field("mMissile", mMissile);
    ar.field("mRangeAttack", mRangeAttack);
    ar.field("mAttackTime", mAttackTime);
    ar.field("mAttack", mAttack);
    ar.field("mAttackOrientation", mAttackOrientation);
    if (ar.reading())
    {
        ar.readStream().readCharacter(&getBoard(), [this](eCharacter *const c)
                                      { mAttackTarget = c; });
    }
    else
    {
        ar.writeStream().writeCharacter(mAttackTarget);
    }
    ar.field("mSpawnTime", mSpawnTime);
    if (ar.reading())
    {
        ar.readStream().readCharacter(&getBoard(), [this](eCharacter *const c)
                                      { mArcher = static_cast<eArcher *>(c); });
    }
    else
    {
        ar.writeStream().writeCharacter(mArcher);
    }
}

void eTower::serializeJson(eJsonArchive& ar) {
    eEmployingBuilding::serializeJson(ar);
    ar.field("mMissile", mMissile);
    ar.field("mRangeAttack", mRangeAttack);
    ar.field("mAttackTime", mAttackTime);
    ar.field("mAttack", mAttack);
    ar.field("mAttackOrientation", mAttackOrientation);
    ar.field("mSpawnTime", mSpawnTime);
    // mAttackTarget and mArcher restored by character load pass
}

bool eTower::spawn()
{
    const auto archer = e::make_shared<eArcher>(getBoard());
    archer->setBothCityIds(cityId());
    archer->setAtlantean(atlantean());
    archer->changeTile(centerTile());
    const auto a = e::make_shared<eArcherAction>(archer.get());
    archer->setAction(a);
    mArcher = archer;
    return true;
}
