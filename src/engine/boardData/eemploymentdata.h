#ifndef EEMPLOYMENTDATA_H
#define EEMPLOYMENTDATA_H

class ePopulationData;
class BoardCity;
class GameBoard;

class eEmploymentData {
public:
    eEmploymentData(const ePopulationData& popData,
                    const BoardCity& city,
                    const GameBoard& board);

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
    const GameBoard& mBoard;
    int mTotalJobVacs{0};
};

#endif // EEMPLOYMENTDATA_H
