#include "ecityfinanceswidget.h"

#include "widgets/elabel.h"
#include "widgets/elinewidget.h"
#include "elanguage.h"
#include "engine/egameboard.h"

eCityFinancesWidget::eCityFinancesWidget(
    eMainWindow * const window,
    eMainWidget * const mw) :
    eInfoWidget(window, mw, false, true) {}

void eCityFinancesWidget::initialize(
    eGameBoard& board, const eCityId cid) {
    const int p = padding();
    const auto title = eLanguage::zeusText(60, 0);
    eInfoWidget::initialize(title);

    const int ww = widgetWidth();
    const int x0 = 3*p;
    const int x1 = x0 + 5*p + ww/3;
    const int x2 = x1 + ww/3;

    const auto w = addCentralWidget();

    const auto col = new eWidget(window());
    col->setNoPadding();

    const auto addFirstRow = [&](const std::string& text) {
        const auto row = new eWidget(window());
        row->setNoPadding();

        const auto l = new eLabel(window());
        l->setSmallFontSize();
        l->setText(text);
        l->setNoPadding();
        l->fitContent();
        row->addWidget(l);

        {
            const auto l = new eLabel(window());
            l->setSmallFontSize();
            l->setText(eLanguage::zeusText(60, 6)); // last year
            l->setNoPadding();
            l->fitContent();
            row->addWidget(l);
            l->setX(x1 - l->width()/2);
        }

        {
            const auto l = new eLabel(window());
            l->setSmallFontSize();
            l->setText(eLanguage::zeusText(60, 7)); // so far this year
            l->setNoPadding();
            l->fitContent();
            row->addWidget(l);
            l->setX(x2 - l->width()/2);
        }

        row->fitContent();
        col->addWidget(row);
    };

    const auto addRow = [&](const std::string& text,
                            const int lastYear,
                            const int soFar) {
        const auto row = new eWidget(window());
        row->setNoPadding();

        const auto l = new eLabel(window());
        l->setSmallFontSize();
        l->setText(text);
        l->setNoPadding();
        l->fitContent();
        row->addWidget(l);
        l->setX(x0);

        {
            const auto l = new eLabel(window());
            l->setSmallFontSize();
            l->setText(std::to_string(lastYear));
            l->setNoPadding();
            l->fitContent();
            row->addWidget(l);
            l->setX(x1);
        }

        {
            const auto l = new eLabel(window());
            l->setSmallFontSize();
            l->setText(std::to_string(soFar));
            l->setNoPadding();
            l->fitContent();
            row->addWidget(l);
            l->setX(x2);
        }

        row->fitContent();
        col->addWidget(row);
    };

    const auto addSpacer = [&]() {
        const auto s = new eWidget(window());
        s->fitContent();
        col->addWidget(s);
    };

    const auto addLine = [&]() {
        const auto w = new eWidget(window());
        w->setNoPadding();

        {
            const auto l = new eLineWidget(window());
            l->setNoPadding();
            l->fitContent();
            l->setWidth(5*p);
            w->addWidget(l);
            l->setX(x1);
        }
        {
            const auto l = new eLineWidget(window());
            l->setNoPadding();
            l->fitContent();
            l->setWidth(5*p);
            w->addWidget(l);
            l->setX(x2);
        }

        w->fitContent();
        col->addWidget(w);
    };

    eCityFinances f;

    {
        const auto pid = board.cityIdToPlayerId(cid);
        const auto p = board.boardPlayerWithId(pid);
        if(p) f = p->finances();
    }

    const auto& lastYear = f.lastYear();
    const auto& soFar = f.thisYear();

    addFirstRow(eLanguage::zeusText(60, 10)); // income
    addRow(eLanguage::zeusText(60, 8), lastYear.fTaxesIn, soFar.fTaxesIn); // taxes in
    addRow(eLanguage::zeusText(60, 9), lastYear.fExports, soFar.fExports); // exports
    addRow(eLanguage::zeusText(60, 20), lastYear.fGiftsReceived, soFar.fGiftsReceived); // gifts received
    addRow(eLanguage::zeusText(60, 24), lastYear.fMinedSilver, soFar.fMinedSilver); // mined silver
    addRow(eLanguage::zeusText(60, 16), lastYear.fTributeReceived, soFar.fTributeReceived); // tribute received
    addRow(eLanguage::zeusText(60, 27), lastYear.fHippodrome, soFar.fHippodrome); // hippodrome
    addLine();
    addRow(eLanguage::zeusText(60, 25), lastYear.totalIncome(), soFar.totalIncome()); // total income

    addSpacer();

    addFirstRow(eLanguage::zeusText(60, 17)); // expenses
    addRow(eLanguage::zeusText(60, 11), lastYear.fImportCosts, soFar.fImportCosts); // import costs
    addRow(eLanguage::zeusText(60, 12), lastYear.fWages, soFar.fWages); // wages
    addRow(eLanguage::zeusText(60, 13), lastYear.fConstruction, soFar.fConstruction); // construction
    addRow(eLanguage::zeusText(60, 21), lastYear.fBribesTributePaid, soFar.fBribesTributePaid); // bribes/tribute paid
    addRow(eLanguage::zeusText(60, 22), lastYear.fGiftsAndAidGiven, soFar.fGiftsAndAidGiven); // gifts and aid given
    addLine();
    addRow(eLanguage::zeusText(60, 26), lastYear.totalExpenses(), soFar.totalExpenses()); // total expenses

    addSpacer();

    addRow(eLanguage::zeusText(60, 18), lastYear.netInOutFlow(), soFar.netInOutFlow()); // net in/out flow

    col->stackVertically();
    col->fitContent();

    w->addWidget(col);
    w->stackHorizontally(p);
}
