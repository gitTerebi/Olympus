#ifndef ETRIREME_H
#define ETRIREME_H

#include "eboatbase.h"
#include "efightingcharacter.h"

class eTriremeAction;

class eTrireme : public eBoatBase,
                 public eFightingCharacter {
public:
    eTrireme(eGameBoard& board);

    static void sPlace(std::vector<eTrireme*> bs,
                       const int ctx, const int cty,
                       eGameBoard& board, const int dist,
                       const int minDistFromEdge);

    eTriremeAction* triremeAction() const;

    eOverlay getSecondaryTexture(const eTileSize size) const override;

    void setSelected(const bool s) { mSelected = s; }
    bool selected() const { return mSelected; }
    bool selectable() const;

    void goHome();
private:
    bool mSelected = false;
};

#endif // ETRIREME_H
