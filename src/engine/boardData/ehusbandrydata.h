#ifndef EHUSBANDRYDATA_H
#define EHUSBANDRYDATA_H

class ePopulationData;
class BoardCity;
class GameBoard;

class eHusbandryData {
public:
    eHusbandryData(const ePopulationData& popData,
                   const BoardCity& city,
                   const GameBoard& board);

    int canSupport() const;
    int storedFood() const;
private:
    const ePopulationData& mPopData;
    const BoardCity& mCity;
    const GameBoard& mBoard;
};

#endif // EHUSBANDRYDATA_H
