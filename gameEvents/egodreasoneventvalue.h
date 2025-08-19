#ifndef EGODREASONEVENTVALUE_H
#define EGODREASONEVENTVALUE_H

class eWriteStream;
class eReadStream;

class eGodReasonEventValue {
public:
    bool godReason() const { return mGodReason; }
    void setGodReason(const bool r) { mGodReason = r; }

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);
protected:
    bool mGodReason = false;
};

#endif // EGODREASONEVENTVALUE_H
