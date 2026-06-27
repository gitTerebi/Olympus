#ifndef EWORLDGOODSWIDGET_H
#define EWORLDGOODSWIDGET_H

#include "ewidget.h"

#include "engine/world-city.h"
#include "pointers/estdselfref.h"
#include "eframedwidget.h"

class eLabel;
class eButton;

class eTradeTypesWidget : public eFramedWidget {
public:
    using eFramedWidget::eFramedWidget;

    void initialize();

    void setTrade(const ePlayerId pid,
                  const std::vector<eResourceTrade>& trade);
};

class eWorldTradeWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const std::string& name, const int fullWidth = 0);
    void setName(const std::string& name);
    void setTrade(const ePlayerId pid,
                  std::vector<eResourceTrade>* const trade);
private:
    std::vector<eResourceTrade>* mTrade = nullptr;

    eLabel* mNameLabel = nullptr;
    eTradeTypesWidget* mTradeTypesWidget = nullptr;;
};

class eWorldGoodsWidget : public eWidget {
public:
    using eWidget::eWidget;

    void initialize();

    void setCity(const stdsptr<WorldCity>& c);
    void setPlayerId(const ePlayerId pid) { mPid = pid; }
private:
    void updateTradeY() const;

    ePlayerId mPid = ePlayerId::neutralFriendly;

    eLabel* mGoodsLabel = nullptr;
    eButton* mOrdersButton = nullptr;

    eWorldTradeWidget* mBuysWidget = nullptr;
    eWorldTradeWidget* mSellsWidget = nullptr;
};

#endif // EWORLDGOODSWIDGET_H
