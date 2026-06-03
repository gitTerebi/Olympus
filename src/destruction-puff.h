#ifndef DESTRUCTION_PUFF_H
#define DESTRUCTION_PUFF_H

#include <memory>
#include <vector>

class GameBoard;
class eSaveArchive;
class eTexture;
enum class eTileSize : int;

class DestructionPuff {
public:
    DestructionPuff(GameBoard& board);
    DestructionPuff(GameBoard& board,
                    double startX, double startY,
                    double dirX, double dirY,
                    int stepCount, int collId);
    ~DestructionPuff();

    void incTime(const int by);
    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;

    double worldX() const { return mWorldX; }
    double worldY() const { return mWorldY; }
    bool dead() const { return mDead; }

    void serialize(eSaveArchive& ar);

private:
    GameBoard& mBoard;
    double mWorldX = 0;
    double mWorldY = 0;
    double mDirX = 0;
    double mDirY = 0;
    int mStepCount = 0;
    int mStepsDone = 0;
    int mCollId = 0;
    int mAnimFrame = 0;
    int mAnimAccum = 0;
    bool mDead = false;
    std::vector<int> mFrameDwell;
};

#endif // DESTRUCTION_PUFF_H
