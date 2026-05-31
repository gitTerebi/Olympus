#ifndef EGODEVENTVALUE_H
#define EGODEVENTVALUE_H

#include <string>

#include "characters/gods/egodtype.h"

class eWriteStream;
class eReadStream;

class eSaveArchive;
class eGodEventValue {
public:
    void serialize(eSaveArchive& ar);

    void setGod(const eGodType god) { mGod = god; }
    eGodType god() const { return mGod; }
protected:
    void longNameReplaceGod(const std::string& id,
                            std::string& tmpl) const;

    eGodType mGod = eGodType::zeus;
};

#endif // EGODEVENTVALUE_H
