#ifndef EWORLDTRIBUTEWIDGET_H
#define EWORLDTRIBUTEWIDGET_H

#include "ewidget.h"

#include "elabel.h"
#include "engine/world-city.h"

class GameBoard;

class eWorldTributeWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize();
    void setBoard(GameBoard* const board) { mBoard = board; }
    void setCity(const stdsptr<WorldCity>& c);
private:
    bool hasClaimToTribute(const stdsptr<WorldCity>& c) const;

    GameBoard* mBoard = nullptr;

    eLabel* mTitleLabel = nullptr;
    eLabel* mTypeIcon = nullptr;
    eLabel* mTextLabel = nullptr;
    eLabel* mAlliesLabel = nullptr;
};

#endif // EWORLDTRIBUTEWIDGET_H
