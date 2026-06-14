#ifndef EGODEVENTVALUE_H
#define EGODEVENTVALUE_H

#include <string>

#include "characters/gods/god-type.h"

class eWriteStream;
class eReadStream;

class eSaveArchive;
class eGodEventValue {
public:
    void serialize(eSaveArchive& ar);

    void setGod(const GodType god) { mGod = god; }
    GodType god() const { return mGod; }
protected:
    void longNameReplaceGod(const std::string& id,
                            std::string& tmpl) const;

    GodType mGod = GodType::zeus;
};

#endif // EGODEVENTVALUE_H
