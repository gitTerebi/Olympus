#ifndef EPALACETILE_H
#define EPALACETILE_H

#include "ebuilding.h"

class ePalace;
class SaveArchive;

class ePalaceTile : public eBuilding {
public:
    ePalaceTile(GameBoard& board,
                const bool other,
                const eCityId cid);

    void erase() override;

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;

    int provide(const eProvide p, const int n) override;

    void setPalace(ePalace* const palace);
    ePalace* palace() const;

    bool other() const { return mOther; }
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    ePalace* mPalace = nullptr;
    const bool mOther;
};

#endif // EPALACETILE_H
