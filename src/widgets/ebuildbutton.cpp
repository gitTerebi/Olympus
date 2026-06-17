#include "ebuildbutton.h"

#include <algorithm>

#include "textures/game-textures.h"
#include "elabel.h"

void eBuildButton::initialize(const std::string& name,
                              const int marbleCost,
                              const int cost,
                              const int customWidth) {
    const auto& intrfc = GameTextures::interface();
    int iRes;
    double mult;
    iResAndMult(iRes, mult);

    const int width = customWidth > 0 ? customWidth : 190*mult;

    setWidth(width);
    setHeight(10*mult);

    const auto& coll = intrfc[iRes];

    {
        const auto nameWidget = new eWidget(window());

        addWidget(nameWidget);
        nameWidget->align(Alignment::vcenter);

        const auto nameLabel = new eLabel(name, window());
        nameLabel->setFontSizeS();
        nameLabel->setNoPadding();
        nameLabel->fitContent();

        nameWidget->addWidget(nameLabel);

        nameWidget->setWidth(std::max(nameLabel->width() + 2*mult,
                                      width/2 + 10*mult));
        nameLabel->align(Alignment::vcenter | Alignment::hcenter);
    }

    if(marbleCost > 0) {
        const auto marbleIcon = new eLabel(window());
        marbleIcon->setTexture(coll.fMarbleUnit);
        marbleIcon->setNoPadding();
        marbleIcon->fitContent();

        const auto cstr = std::to_string(marbleCost);
        const auto marbleText = new eLabel(cstr, window());
        marbleText->setFontSizeS();
        marbleText->setNoPadding();
        marbleText->fitContent();

        addWidget(marbleIcon);
        addWidget(marbleText);

        marbleIcon->align(Alignment::vcenter);
        marbleText->align(Alignment::vcenter);

        marbleIcon->setX(width - 65*mult);
        marbleText->setX(marbleIcon->x() + marbleIcon->width());
    }
    if(cost > 0) {
        const auto drachmaIcon = new eScaledTextureLabel(window());
        drachmaIcon->setTexture(coll.fDrachmasUnit);
        drachmaIcon->setNoPadding();
        drachmaIcon->fitContent();
        drachmaIcon->setFitToDrawSize(true);
        drachmaIcon->setMaxDrawHeight(10*mult);

        const auto cstr = std::to_string(cost);
        const auto drachmaText = new eLabel(cstr, window());
        drachmaText->setFontSizeS();
        drachmaText->setNoPadding();
        drachmaText->fitContent();

        addWidget(drachmaIcon);
        addWidget(drachmaText);

        drachmaIcon->align(Alignment::vcenter);
        drachmaText->align(Alignment::vcenter);

        drachmaIcon->setX(width - 35*mult);
        drachmaText->setX(drachmaIcon->x() + drachmaIcon->width());
    }
}
