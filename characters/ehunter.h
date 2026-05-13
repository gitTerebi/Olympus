#ifndef EHUNTER_H
#define EHUNTER_H

#include "eresourcecollector.h"

class eSaveArchive;

class eHunter : public eResourceCollector {
public:
    eHunter(eGameBoard& board);

    bool deerHunter() const { return mDeerHunter; }
    void setDeerHunter(const bool h);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
void serializeJson(eJsonArchive& ar) override {
        eResourceCollector::serializeJson(ar);
        bool deerHunter = mDeerHunter;
        ar.field("deerHunter", deerHunter);
        if(ar.reading()) setDeerHunter(deerHunter);
}

private:
    void serialize(eSaveArchive& ar);

    bool mDeerHunter{false};
};

#endif // EHUNTER_H
