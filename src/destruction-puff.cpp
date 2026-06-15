#include "destruction-puff.h"

#include "engine/game-board.h"
#include "textures/game-textures.h"
#include "fileIO/save-archive.h"
#include "rand.h"

static std::vector<int> randomFrameDwell() {
    std::vector<int> dwell(9);
    for(int i = 0; i < 9; i++)
        dwell[i] = 150 + (std::abs(Rand::rand()) % 201);
    return dwell;
}

DestructionPuff::DestructionPuff(GameBoard& board) :
    mBoard(board) {
    GameTextures::loadDust();
    board.registerDestructionPuff(this);
}

DestructionPuff::DestructionPuff(GameBoard& board,
                                 const double startX, const double startY,
                                 const double dirX, const double dirY,
                                 const int stepCount, const int collId) :
    mBoard(board),
    mWorldX(startX), mWorldY(startY),
    mDirX(dirX), mDirY(dirY),
    mStepCount(stepCount), mCollId(collId),
    mFrameDwell(randomFrameDwell()) {
    GameTextures::loadDust();
    board.registerDestructionPuff(this);
}

DestructionPuff::~DestructionPuff() {
    mBoard.unregisterDestructionPuff(this);
}

void DestructionPuff::incTime(const int by) {
    if(mDead) return;
    mAnimAccum += by;
    while(mAnimFrame < 9 && mAnimAccum >= mFrameDwell[mAnimFrame]) {
        mAnimAccum -= mFrameDwell[mAnimFrame];
        if(mStepsDone < mStepCount) {
            mWorldX += mDirX * 0.5;
            mWorldY += mDirY * 0.5;
            mStepsDone++;
        }
        mAnimFrame++;
    }
    if(mAnimFrame >= 9) mDead = true;
}

std::shared_ptr<Texture>
DestructionPuff::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& textures = GameTextures::destrution();
    const auto& colls = textures[id].fDust;
    const auto& coll = colls[mCollId % static_cast<int>(colls.size())];
    return coll.getTexture(mAnimFrame % coll.size());
}

void DestructionPuff::serialize(SaveArchive& ar) {
    ar.field("worldX", mWorldX);
    ar.field("worldY", mWorldY);
    ar.field("dirX", mDirX);
    ar.field("dirY", mDirY);
    ar.field("stepCount", mStepCount);
    ar.field("stepsDone", mStepsDone);
    ar.field("collId", mCollId);
    ar.field("animFrame", mAnimFrame);
    ar.field("animAccum", mAnimAccum);
    ar.field("dead", mDead);
    ar.arrayField("frameDwell", mFrameDwell,
        [](SaveArchive& itemAr, int& v) { itemAr.field("dwell", v); });
}
