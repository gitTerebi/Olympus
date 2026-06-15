#ifndef EGODREASONEVENTVALUE_H
#define EGODREASONEVENTVALUE_H

class WriteStream;
class ReadStream;

class SaveArchive;
class eGodReasonEventValue {
public:
    bool godReason() const { return mGodReason; }
    void setGodReason(const bool r) { mGodReason = r; }

    void serialize(SaveArchive& ar);
protected:
    bool mGodReason = false;
};

#endif // EGODREASONEVENTVALUE_H
