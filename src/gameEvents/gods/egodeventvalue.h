#ifndef EGODEVENTVALUE_H
#define EGODEVENTVALUE_H

#include <string>

#include "characters/gods/god-type.h"

class WriteStream;
class ReadStream;

class SaveArchive;
class eGodEventValue {
public:
    void serialize(SaveArchive& ar);

    void setGod(const GodType god) { mGod = god; }
    GodType god() const { return mGod; }
protected:
    void longNameReplaceGod(const std::string& id,
                            std::string& tmpl) const;

    GodType mGod = GodType::zeus;
};

#endif // EGODEVENTVALUE_H
