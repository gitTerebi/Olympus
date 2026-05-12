#ifndef EPLANNEDACTION_H
#define EPLANNEDACTION_H

class eReadStream;
class eWriteStream;
class eJsonArchive;

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

    virtual void read(eReadStream& src, eGameBoard& board);
    virtual void write(eWriteStream& dst) const;
    virtual void serializeJson(eJsonArchive& ar, eGameBoard& board);

    static ePlannedAction* sCreate(const ePlannedActionType type);

    ePlannedActionType type() const { return mType; }

    void incTime(const int by, eGameBoard& board);
    bool finished() const { return mFinished; }
protected:
    void serialize(eSaveArchive& ar);
private:
    const ePlannedActionType mType;
    bool mRecurring;
    int mActionTime;
    bool mFinished = false;
    int mTime = 0;
};

#endif // EPLANNEDACTION_H
