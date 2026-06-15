#ifndef EPLANNEDACTION_H
#define EPLANNEDACTION_H

class GameBoard;
class SaveArchive;

enum class ePlannedActionType {
    regrowForest,
    colonyMonument
};

class ePlannedAction {
public:
    ePlannedAction(const bool recurring,
                   const int actionTime,
                   const ePlannedActionType type);
    ePlannedAction(const ePlannedActionType type);
    virtual ~ePlannedAction();

    virtual void trigger(GameBoard& board) = 0;

    void serialize(SaveArchive& ar, GameBoard* board);

    static ePlannedAction* sCreate(const ePlannedActionType type);

    ePlannedActionType type() const { return mType; }

    void incTime(const int by, GameBoard& board);
    bool finished() const { return mFinished; }
protected:
    virtual void serializeFields(SaveArchive& ar, GameBoard* board);
private:
    const ePlannedActionType mType;
    bool mRecurring;
    int mActionTime;
    bool mFinished = false;
    int mTime = 0;
};

#endif // EPLANNEDACTION_H
