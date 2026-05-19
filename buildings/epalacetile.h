#ifndef EPALACETILE_H
#define EPALACETILE_H

#include "ebuilding.h"

class ePalace;
class eSaveArchive;

class ePalaceTile : public eBuilding {
public:
    ePalaceTile(eGameBoard& board,
                const bool other,
                const eCityId cid);

    void erase() override;

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    int provide(const eProvide p, const int n) override;

    void setPalace(ePalace* const palace);
    ePalace* palace() const;

    bool other() const { return mOther; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    ePalace* mPalace = nullptr;
    const bool mOther;
};

#endif // EPALACETILE_H
