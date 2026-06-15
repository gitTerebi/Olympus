#ifndef EDIRECTIONLASTUSETIME_H
#define EDIRECTIONLASTUSETIME_H

#include "engine/eorientation.h"

#include <map>
#include <vector>

class eTile;
class SaveArchive;

class eDirectionLastUseTime {
public:
    eDirectionLastUseTime() {
        for(int i = 0; i < 8; i++) {
            mTime.push_back(0);
        }
    }

    int& time(const eOrientation o) {
        const int id = static_cast<int>(o);
        return mTime[id];
    }

    void serialize(SaveArchive& ar);
private:
    std::vector<int> mTime;
};

using eDirectionTimes = std::map<eTile*, eDirectionLastUseTime>;

#endif // EDIRECTIONLASTUSETIME_H
