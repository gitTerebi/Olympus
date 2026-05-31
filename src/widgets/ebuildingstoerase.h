#ifndef EBUILDINGSTOERASE_H
#define EBUILDINGSTOERASE_H

#include <vector>
#include <set>

class eBuilding;
class eCharacter;
class eTile;

class eBuildingsToErase {
public:
    eBuildingsToErase() {}

    void addBuilding(eBuilding* const b);
    void addCharacter(eCharacter* const c);

    int erase(const bool important);

    bool hasImportantBuildings() const;
    bool hasNonEmptyAgoras() const;
private:
    void erase(eBuilding* const b);

    std::set<eBuilding*> mBs;
    std::set<eBuilding*> mImpBs;
    std::set<eBuilding*> mAgoBs;
    std::set<eCharacter*> mCs;
};

#endif // EBUILDINGSTOERASE_H
