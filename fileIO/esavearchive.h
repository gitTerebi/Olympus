#ifndef ESAVEARCHIVE_H
#define ESAVEARCHIVE_H

#include <functional>
#include <memory>

#include "estreams.h"

class eGameBoard;
class eCharacterAction;
class eTile;
enum class eCharActionType;

class eSaveArchive {
public:
    explicit eSaveArchive(eReadStream& src) : mSrc(&src) {}
    explicit eSaveArchive(eWriteStream& dst) : mDst(&dst) {}

    bool reading() const { return mSrc; }
    bool writing() const { return mDst; }
    bool versionAtLeast(const int version) const {
        return writing() || mSrc->formatVersion() >= version;
    }

    eReadStream& readStream() const { return *mSrc; }
    eWriteStream& writeStream() const { return *mDst; }

    template <typename T>
    void value(T& value) {
        if(reading()) {
            *mSrc >> value;
        } else {
            *mDst << value;
        }
    }

    template <typename T>
    void valueSince(const int version, T& value, const T& defaultValue) {
        if(versionAtLeast(version)) {
            this->value(value);
        } else {
            value = defaultValue;
        }
    }

    void tile(eTile*& tile, eGameBoard& board) {
        if(reading()) {
            tile = mSrc->readTile(board);
        } else {
            mDst->writeTile(tile);
        }
    }

    template <typename T>
    void characterAction(std::shared_ptr<T>& action, const std::function<std::shared_ptr<T>(eCharActionType)>& create) {
        bool hasAction = action != nullptr;
        this->value(hasAction);
        if(!hasAction) {
            if(reading()) action = nullptr;
            return;
        }

        if(reading()) {
            eCharActionType type;
            this->value(type);
            action = create(type);
            action->read(*mSrc);
        } else {
            eCharActionType type = action->type();
            this->value(type);
            action->write(*mDst);
        }
    }

private:
    eReadStream* mSrc = nullptr;
    eWriteStream* mDst = nullptr;
};

#endif // ESAVEARCHIVE_H
