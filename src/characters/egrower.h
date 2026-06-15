#ifndef EGROWER_H
#define EGROWER_H

#include "echaracter.h"

enum class eGrowerType {
    grapesAndOlives,
    oranges
};

class CharacterTextures;

class eGrower : public eCharacter {
public:
    eGrower(GameBoard& board);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;

    void setGrowerType(const eGrowerType t) { mType = t; }
    eGrowerType growerType() const { return mType; }

    int olives() const { return mOlives; }
    int grapes() const { return mGrapes; }
    int oranges() const { return mOranges; }

    void incGrapes(const int i = 1);
    void incOlives(const int i = 1);
    void incOranges(const int i = 1);

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    std::shared_ptr<Texture> getGrapesAndOlivesTex(
            const CharacterTextures& texs) const;
    std::shared_ptr<Texture> getOrangesTex(
            const CharacterTextures& texs) const;

    eGrowerType mType = eGrowerType::grapesAndOlives;
    int mOlives = 0;
    int mGrapes = 0;
    int mOranges = 0;
};

#endif // EGROWER_H
