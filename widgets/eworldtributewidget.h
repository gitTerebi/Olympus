#ifndef EWORLDTRIBUTEWIDGET_H
#define EWORLDTRIBUTEWIDGET_H

#include "ewidget.h"

#include "elabel.h"
#include "engine/eworldcity.h"

class eGameBoard;

class eWorldTributeWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize();
    void setBoard(eGameBoard* const board) { mBoard = board; }
    void setCity(const stdsptr<eWorldCity>& c);
private:
    bool hasClaimToTribute(const stdsptr<eWorldCity>& c) const;

    eGameBoard* mBoard = nullptr;

    eLabel* mTitleLabel = nullptr;
    eLabel* mTypeIcon = nullptr;
    eLabel* mTextLabel = nullptr;
    eLabel* mAlliesLabel = nullptr;
};

#endif // EWORLDTRIBUTEWIDGET_H
