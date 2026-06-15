#ifndef ECITYFINANCES_H
#define ECITYFINANCES_H

#include "fileIO/save-archive.h"

enum class eFinanceTarget {
    // income
    taxesIn,
    exports,
    giftsReceived,
    minedSilver,
    tributeReceived,
    hippodrome,

    // expenses
    importCosts,
    wages,
    construction,
    bribesTributePaid,
    giftsAndAidGiven
};

struct eFinanceYear {
    void add(const eFinanceYear& src) {
        // income
        fTaxesIn += src.fTaxesIn;
        fExports += src.fExports;
        fGiftsReceived += src.fGiftsReceived;
        fMinedSilver += src.fMinedSilver;
        fTributeReceived += src.fTributeReceived;
        fHippodrome += src.fHippodrome;

        // expenses
        fImportCosts += src.fImportCosts;
        fWages += src.fWages;
        fConstruction += src.fConstruction;
        fBribesTributePaid += src.fBribesTributePaid;
        fGiftsAndAidGiven += src.fGiftsAndAidGiven;
    }

    void add(const int d, const eFinanceTarget t) {
        switch(t) {
        // income
        case eFinanceTarget::taxesIn:
            fTaxesIn += d;
            break;
        case eFinanceTarget::exports:
            fExports += d;
            break;
        case eFinanceTarget::giftsReceived:
            fGiftsReceived += d;
            break;
        case eFinanceTarget::minedSilver:
            fMinedSilver += d;
            break;
        case eFinanceTarget::tributeReceived:
            fTributeReceived += d;
            break;
        case eFinanceTarget::hippodrome:
            fHippodrome += d;
            break;

        // expenses
        case eFinanceTarget::importCosts:
            fImportCosts -= d;
            break;
        case eFinanceTarget::wages:
            fWages -= d;
            break;
        case eFinanceTarget::construction:
            fConstruction -= d;
            break;
        case eFinanceTarget::bribesTributePaid:
            fBribesTributePaid -= d;
            break;
        case eFinanceTarget::giftsAndAidGiven:
            fGiftsAndAidGiven -= d;
            break;
        }
    }

    void serialize(SaveArchive& ar) {
        ar.field("taxesIn", fTaxesIn, 0);
        ar.field("exports", fExports, 0);
        ar.field("giftsReceived", fGiftsReceived, 0);
        ar.field("minedSilver", fMinedSilver, 0);
        ar.field("tributeReceived", fTributeReceived, 0);
        ar.field("hippodrome", fHippodrome, 0);
        ar.field("importCosts", fImportCosts, 0);
        ar.field("wages", fWages, 0);
        ar.field("construction", fConstruction, 0);
        ar.field("bribesTributePaid", fBribesTributePaid, 0);
        ar.field("giftsAndAidGiven", fGiftsAndAidGiven, 0);
    }

    int totalIncome() const {
        return fTaxesIn + fExports + fGiftsReceived +
               fMinedSilver + fTributeReceived + fHippodrome;
    }

    int totalExpenses() const {
        return fImportCosts + fWages + fConstruction +
               fBribesTributePaid + fGiftsAndAidGiven;
    }

    int netInOutFlow() const {
        return totalIncome() - totalExpenses();
    }

    // income
    int fTaxesIn = 0;
    int fExports = 0;
    int fGiftsReceived = 0;
    int fMinedSilver = 0;
    int fTributeReceived = 0;
    int fHippodrome = 0;

    // expenses
    int fImportCosts = 0;
    int fWages = 0;
    int fConstruction = 0;
    int fBribesTributePaid = 0;
    int fGiftsAndAidGiven = 0;
};

class eCityFinances {
public:
    eCityFinances();

    void add(const int d, const eFinanceTarget t);

    void nextYear();

    const eFinanceYear& lastYear() const { return mLastYear; }
    const eFinanceYear& thisYear() const { return mThisYear; }

    void serialize(SaveArchive& ar);
private:
    eFinanceYear mLastYear;
    eFinanceYear mThisYear;
};

#endif // ECITYFINANCES_H
