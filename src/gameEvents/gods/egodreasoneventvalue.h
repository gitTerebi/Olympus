#ifndef EGODREASONEVENTVALUE_H
#define EGODREASONEVENTVALUE_H

class eWriteStream;
class eReadStream;

class eSaveArchive;
class eGodReasonEventValue {
public:
    bool godReason() const { return mGodReason; }
    void setGodReason(const bool r) { mGodReason = r; }

    void serialize(eSaveArchive& ar);
protected:
    bool mGodReason = false;
};

#endif // EGODREASONEVENTVALUE_H
