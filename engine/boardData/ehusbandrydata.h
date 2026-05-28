#ifndef EHUSBANDRYDATA_H
#define EHUSBANDRYDATA_H

class ePopulationData;
class BoardCity;
class eGameBoard;

class eHusbandryData {
public:
    eHusbandryData(const ePopulationData& popData,
                   const BoardCity& city,
                   const eGameBoard& board);

    int canSupport() const;
    int storedFood() const;
private:
    const ePopulationData& mPopData;
    const BoardCity& mCity;
    const eGameBoard& mBoard;
};

#endif // EHUSBANDRYDATA_H
