#ifndef EGOD_H
#define EGOD_H

#include "characters/echaracter.h"

#include "god-type.h"

#include "textures/destruction-textures.h"

class eSaveArchive;
enum class eBuildingType;

enum class GodQuestId {
    godQuest1,
    godQuest2
};

enum class GodAttitude {
    friendly,
    hostile,
    worshipped
};

class God : public eCharacter {
public:
    God(GameBoard& board, const GodType gt);

    static GodType sCharacterToGodType(const eCharacterType type,
                                        bool* const valid = nullptr);
    static eCharacterType sGodToCharacterType(const GodType type);
    static stdsptr<God> sCreateGod(const GodType type, GameBoard& board);

    using eTexPtr = std::vector<eTextureCollection> DestructionTextures::*;
    static eTexPtr sGodMissile(const GodType gt);

    static GodType sFightWinner(const GodType g1, const GodType g2);
    static std::string sFightResultString(const GodType g1, const GodType g2);

    static int sGodAttackTime(const GodType gt);
    static int sGodBlessTime(const GodType gt);
    static int sGodAppearTime(const GodType gt);

    static bool sTarget(const GodType gt, const eBuildingType bt);

    static std::string sGodName(const GodType gt);

    static void sGodStrings(std::vector<GodType>& gods,
                            std::vector<std::string>& godNames);

    static void sLoadTextures(const GodType g);

    GodType type() const { return mType; }

    GodAttitude attitude() const { return mAttitude; }
    void setAttitude(const GodAttitude attitude) { mAttitude = attitude; }

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    const GodType mType;
    GodAttitude mAttitude = GodAttitude::friendly;
};

#endif // EGOD_H
