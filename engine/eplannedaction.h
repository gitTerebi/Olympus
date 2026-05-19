#ifndef EPLANNEDACTION_H
#define EPLANNEDACTION_H

class eGameBoard;
class eSaveArchive;

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

    virtual void trigger(eGameBoard& board) = 0;

    void serialize(eSaveArchive& ar, eGameBoard* board);

    static ePlannedAction* sCreate(const ePlannedActionType type);

    ePlannedActionType type() const { return mType; }

    void incTime(const int by, eGameBoard& board);
    bool finished() const { return mFinished; }
protected:
    virtual void serializeFields(eSaveArchive& ar, eGameBoard* board);
private:
    const ePlannedActionType mType;
    bool mRecurring;
    int mActionTime;
    bool mFinished = false;
    int mTime = 0;
};

#endif // EPLANNEDACTION_H
