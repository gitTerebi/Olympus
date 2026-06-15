#include "eworldtributewidget.h"

#include "language.h"
#include "engine/game-board.h"
#include "engine/tribute.h"
#include "vector-helpers.h"

void eWorldTributeWidget::initialize() {
    const auto titleStr = Language::zeusText(44, 320);
    mTitleLabel = new eLabel(titleStr, window());
    mTitleLabel->setFontSizeXS();
    mTitleLabel->setNoPadding();
    mTitleLabel->fitContent();
    addWidget(mTitleLabel);
    mTitleLabel->align(Alignment::hcenter);

    mTypeIcon = new eLabel(window());
    mTypeIcon->setNoPadding();
    addWidget(mTypeIcon);
    mTypeIcon->setY(mTitleLabel->height());

    mTextLabel = new eLabel(window());
    mTextLabel->setFontSizeXS();
    mTextLabel->setNoPadding();
    addWidget(mTextLabel);
    mTextLabel->setY(mTitleLabel->height());

    mAlliesLabel = new eLabel(window());
    mAlliesLabel->setFontSizeXS();
    mAlliesLabel->setNoPadding();
    addWidget(mAlliesLabel);
    const auto alliesText = Language::zeusText(44, 323);
    mAlliesLabel->resize(width(), height());
    mAlliesLabel->setWrapWidth(width());
    mAlliesLabel->setWrapAlignment(Alignment::hcenter);
    mAlliesLabel->setText(alliesText);
    mAlliesLabel->hide();
}

bool eWorldTributeWidget::hasClaimToTribute(
        const stdsptr<WorldCity>& c) const {
    if(!mBoard || !c) return false;
    const auto cids = mBoard->personPlayerCitiesOnBoard();
    const auto defs = mBoard->conqueredBy(c->cityId());
    for(const auto cid : cids) {
        const auto playerCity = mBoard->world().cityWithId(cid);
        if(VectorHelpers::contains(defs, playerCity)) return true;
    }
    return false;
}

void eWorldTributeWidget::setCity(const stdsptr<WorldCity>& c) {
    mAlliesLabel->hide();
    mTitleLabel->hide();
    mTypeIcon->hide();
    mTextLabel->hide();
    if(c) {
        if(c->isOnBoardColony()) return;
        const bool isDist = c->isDistant();
        if(isDist) {
            mAlliesLabel->setText(Language::zeusText(44, 325));
            mAlliesLabel->show();
            return;
        }
        const bool cityOwesTribute = c->isVassal() || c->isColony();
        const bool playerPaysTribute = hasClaimToTribute(c);
        const bool tributeCity = cityOwesTribute || playerPaysTribute;
        if(!tributeCity) {
            if(c->isAlly()) {
                mAlliesLabel->setText(Language::zeusText(44, 323));
                mAlliesLabel->show();
            } else if(c->isRival()) {
                mAlliesLabel->setText(Language::zeusText(44, 324));
                mAlliesLabel->show();
            }
            return;
        }
        mTitleLabel->show();
        mTypeIcon->show();
        mTextLabel->show();
        eResourceType ttype;
        int count;
        if(playerPaysTribute) {
            const auto diff = mBoard->personPlayerDifficulty();
            const auto tribute = TributeHelpers::payTribute(*c, diff);
            ttype = tribute.fType;
            count = tribute.fCount;
        } else {
            const auto tribute = TributeHelpers::receiveTribute(*c);
            ttype = tribute.fType;
            count = tribute.fCount;
        }
        const auto uiScale = resolution().uiScale();
        const auto icon = eResourceTypeHelpers::icon(uiScale, ttype);
        mTypeIcon->setTexture(icon);
        mTypeIcon->fitContent();
        const auto name = eResourceTypeHelpers::typeName(ttype);
        const auto yearly = Language::zeusText(44, 322);
        const auto text = std::to_string(count) + " " + name + " " + yearly;
        mTextLabel->setText(text);
        mTextLabel->fitContent();
        mTextLabel->setX(mTypeIcon->x() + mTypeIcon->width());
    }
}
