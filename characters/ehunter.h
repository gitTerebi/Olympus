#ifndef EHUNTER_H
#define EHUNTER_H

#include "eresourcecollector.h"

class eSaveArchive;

class eHunter : public eResourceCollector {
public:
    eHunter(eGameBoard& board);

    bool deerHunter() const { return mDeerHunter; }
    void setDeerHunter(const bool h);

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool mDeerHunter{false};
};

#endif // EHUNTER_H
