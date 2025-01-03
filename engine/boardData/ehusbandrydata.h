#ifndef EHUSBANDRYDATA_H
#define EHUSBANDRYDATA_H

class ePopulationData;
class eBoardCity;
class eGameBoard;

class eHusbandryData {
public:
    eHusbandryData(const ePopulationData& popData,
                   const eBoardCity& city,
                   const eGameBoard& board);

    int canSupport() const;
    int storedFood() const;
private:
    const ePopulationData& mPopData;
    const eBoardCity& mCity;
    const eGameBoard& mBoard;
};

#endif // EHUSBANDRYDATA_H
