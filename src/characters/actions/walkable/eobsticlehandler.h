#ifndef EOBSTICLEHANDLER_H
#define EOBSTICLEHANDLER_H

#include "pointers/estdpointer.h"
#include "fileIO/estreams.h"
#include "fileIO/esavearchive.h"

class eTile;

enum class eObsticleHandlerType {
    monster,
    god,
    soldier
};

class eObsticleHandler {
public:
    eObsticleHandler(GameBoard& board,
                     const eObsticleHandlerType type) :
        mBoard(board), mType(type) {}
    virtual ~eObsticleHandler() = default;

    virtual bool handle(eTile* const tile) = 0;

    void serialize(eSaveArchive& ar) { serializeFields(ar); }

    GameBoard& board() const { return mBoard; }
    eObsticleHandlerType type() const { return mType; }

    static stdsptr<eObsticleHandler> sCreate(
            GameBoard& board,
            const eObsticleHandlerType type);
protected:
    virtual void serializeFields(eSaveArchive& ar) { (void)ar; }
private:
    GameBoard& mBoard;
    const eObsticleHandlerType mType;
};

inline bool obsticleHandlerField(eSaveArchive& ar, const char* name,
                                 GameBoard& board,
                                 stdsptr<eObsticleHandler>& val) {
    bool hasValue = val != nullptr;
    const std::string hasName = std::string(name) + ".has";
    ar.field(hasName.c_str(), hasValue, false);
    if(!hasValue) {
        if(ar.reading()) val = nullptr;
        return true;
    }
    eObsticleHandlerType type = ar.writing() ? val->type() : eObsticleHandlerType::monster;
    const std::string typeName = std::string(name) + ".type";
    ar.field(typeName.c_str(), type);
    if(ar.reading()) val = eObsticleHandler::sCreate(board, type);
    if(!val) {
        printf("[saveLoad] obsticleHandlerField '%s' unknown type %d.\n",
               name, static_cast<int>(type));
        return false;
    }
    const bool ok = ar.archiveField(name, [&](eSaveArchive& childAr) {
        val->serialize(childAr);
    });
    if(!ok) printf("[saveLoad] obsticleHandlerField '%s' missing data.\n", name);
    return ok;
}

#endif // EOBSTICLEHANDLER_H
