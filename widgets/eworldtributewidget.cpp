#include "eworldtributewidget.h"

#include "elanguage.h"
#include "engine/egameboard.h"
#include "evectorhelpers.h"

void eWorldTributeWidget::initialize() {
    const auto titleStr = eLanguage::zeusText(44, 320);
    mTitleLabel = new eLabel(titleStr, window());
    mTitleLabel->setFontSizeXS();
    mTitleLabel->setNoPadding();
    mTitleLabel->fitContent();
    addWidget(mTitleLabel);
    mTitleLabel->align(eAlignment::hcenter);

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
    const auto alliesText = eLanguage::zeusText(44, 323);
    mAlliesLabel->resize(width(), height());
    mAlliesLabel->setWrapWidth(width());
    mAlliesLabel->setWrapAlignment(eAlignment::hcenter);
    mAlliesLabel->setText(alliesText);
    mAlliesLabel->hide();
}

bool eWorldTributeWidget::hasClaimToTribute(
        const stdsptr<eWorldCity>& c) const {
    if(!mBoard || !c) return false;
    const auto cids = mBoard->personPlayerCitiesOnBoard();
    for(const auto cid : cids) {
        const auto defs = mBoard->defeatedBy(cid);
        if(eVectorHelpers::contains(defs, c)) return true;
    }
    return false;
}

void eWorldTributeWidget::setCity(const stdsptr<eWorldCity>& c) {
    mAlliesLabel->hide();
    mTitleLabel->hide();
    mTypeIcon->hide();
    mTextLabel->hide();
    if(c) {
        if(c->isOnBoardColony()) return;
        const bool isDist = c->isDistant();
        if(isDist) {
            mAlliesLabel->setText(eLanguage::zeusText(44, 325));
            mAlliesLabel->show();
            return;
        }
        const bool claimsTribute = hasClaimToTribute(c);
        const bool tributeCity = c->isVassal() || c->isColony() ||
                                 claimsTribute;
        if(!tributeCity) {
            if(c->isAlly()) {
                mAlliesLabel->setText(eLanguage::zeusText(44, 323));
                mAlliesLabel->show();
            } else if(c->isRival()) {
                mAlliesLabel->setText(eLanguage::zeusText(44, 324));
                mAlliesLabel->show();
            }
            return;
        }
        mTitleLabel->show();
        mTypeIcon->show();
        mTextLabel->show();
        const auto ttype = claimsTribute ? c->recTributeType() :
                           c->tributeType();
        const int count = claimsTribute ? c->recTributeCount() :
                          c->tributeCount();
        const auto uiScale = resolution().uiScale();
        const auto icon = eResourceTypeHelpers::icon(uiScale, ttype);
        mTypeIcon->setTexture(icon);
        mTypeIcon->fitContent();
        const auto name = eResourceTypeHelpers::typeName(ttype);
        const auto yearly = eLanguage::zeusText(44, 322);
        const auto text = std::to_string(count) + " " + name + " " + yearly;
        mTextLabel->setText(text);
        mTextLabel->fitContent();
        mTextLabel->setX(mTypeIcon->x() + mTypeIcon->width());
    }
}
