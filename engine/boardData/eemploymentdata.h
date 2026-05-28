#ifndef EEMPLOYMENTDATA_H
#define EEMPLOYMENTDATA_H

class ePopulationData;
class BoardCity;
class eGameBoard;

class eEmploymentData {
public:
    eEmploymentData(const ePopulationData& popData,
                    const BoardCity& city,
                    const eGameBoard& board);

    void incTotalJobVacancies(const int v);

    int employable() const;
    int employed() const;
    int pensions() const;
    int unemployed() const;
    int freeJobVacancies() const;
    int totalJobVacancies() const;

    double vacanciesFilledFraction() const;
    double employedFraction() const;
private:
    const ePopulationData& mPopData;
    const BoardCity& mCity;
    const eGameBoard& mBoard;
    int mTotalJobVacs{0};
};

#endif // EEMPLOYMENTDATA_H
