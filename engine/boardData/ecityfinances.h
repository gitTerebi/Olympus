#ifndef ECITYFINANCES_H
#define ECITYFINANCES_H

#include "fileIO/ereadstream.h"
#include "fileIO/ewritestream.h"

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

    void read(eReadStream& src) {
        // income
        src >> fTaxesIn;
        src >> fExports;
        src >> fGiftsReceived;
        src >> fMinedSilver;
        src >> fTributeReceived;
        src >> fHippodrome;

        // expenses
        src >> fImportCosts;
        src >> fWages;
        src >> fConstruction;
        src >> fBribesTributePaid;
        src >> fGiftsAndAidGiven;
    }

    void write(eWriteStream& dst) const {
        // income
        dst << fTaxesIn;
        dst << fExports;
        dst << fGiftsReceived;
        dst << fMinedSilver;
        dst << fTributeReceived;
        dst << fHippodrome;

        // expenses
        dst << fImportCosts;
        dst << fWages;
        dst << fConstruction;
        dst << fBribesTributePaid;
        dst << fGiftsAndAidGiven;
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

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
private:
    eFinanceYear mLastYear;
    eFinanceYear mThisYear;
};

#endif // ECITYFINANCES_H
