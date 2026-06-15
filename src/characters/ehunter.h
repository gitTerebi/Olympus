#ifndef EHUNTER_H
#define EHUNTER_H

#include "eresourcecollector.h"

class SaveArchive;

class eHunter : public eResourceCollector {
public:
    eHunter(GameBoard& board);

    bool deerHunter() const { return mDeerHunter; }
    void setDeerHunter(const bool h);

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    bool mDeerHunter{false};
};

#endif // EHUNTER_H
