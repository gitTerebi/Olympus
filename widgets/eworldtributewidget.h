#ifndef EWORLDTRIBUTEWIDGET_H
#define EWORLDTRIBUTEWIDGET_H

#include "ewidget.h"

#include "elabel.h"
#include "engine/e-worldcity.h"

class GameBoard;

class eWorldTributeWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize();
    void setBoard(GameBoard* const board) { mBoard = board; }
    void setCity(const stdsptr<eWorldCity>& c);
private:
    bool hasClaimToTribute(const stdsptr<eWorldCity>& c) const;

    GameBoard* mBoard = nullptr;

    eLabel* mTitleLabel = nullptr;
    eLabel* mTypeIcon = nullptr;
    eLabel* mTextLabel = nullptr;
    eLabel* mAlliesLabel = nullptr;
};

#endif // EWORLDTRIBUTEWIDGET_H
