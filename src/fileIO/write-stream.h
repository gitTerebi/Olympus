#ifndef WRITE_STREAM_H
#define WRITE_STREAM_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cassert>

class eTile;
class eBuilding;
class eCharacter;
class eCharacterAction;
class eCharacterActionFunction;
class eGodAct;
class WorldCity;
class Banner;
class SoldierBanner;
class eGameEvent;
class eInvasionHandler;

#include "engine/edirectionlastusetime.h"

class WriteTarget {
public:
    WriteTarget(std::ofstream* const file) :
        fFile(file) {}
    WriteTarget(void* mem) :
        fMem(mem) {}
    WriteTarget(std::vector<char>* const vec) :
        fVec(vec) {}

    inline size_t write(const void* const data, const size_t len) {
        assert(fFile || fMem || fVec);
        if(fFile) {
            fFile->write(static_cast<const char*>(data), len);
            return len;
        } else if(fMem) {
            std::memcpy(static_cast<char*>(fMem) + fMemPos, data, len);
            fMemPos += len;
            return len;
        } else if(fVec) {
            const auto oldSize = fVec->size();
            fVec->resize(oldSize + len);
            std::memcpy(fVec->data() + oldSize, data, len);
            fMemPos += len;
            return len;
        }
        return 0;
    }

    size_t memPos() const { return fMemPos; }
private:
    std::ofstream* fFile = nullptr;
    void* fMem = nullptr;
    std::vector<char>* fVec = nullptr;
    size_t fMemPos = 0;
};

class WriteStream {
public:
    WriteStream(const WriteTarget& dst);

    void writeFormat(const std::string& format);
    void setFormat(const std::string& format) { mFormat = format; }
    const std::string& format() const { return mFormat; }

    inline size_t write(const void* const data, const size_t len) {
        return mDst.write(data, len);
    }

    inline WriteStream& operator<<(const bool val) {
        write(&val, sizeof(bool));
        return *this;
    }

    inline WriteStream& operator<<(const unsigned char val) {
        write(&val, sizeof(unsigned char));
        return *this;
    }

    inline WriteStream& operator<<(const char val) {
        write(&val, sizeof(char));
        return *this;
    }

    inline WriteStream& operator<<(const float val) {
        write(&val, sizeof(float));
        return *this;
    }

    inline WriteStream& operator<<(const double val) {
        write(&val, sizeof(double));
        return *this;
    }

    inline WriteStream& operator<<(const int32_t val) {
        write(&val, sizeof(int32_t));
        return *this;
    }

    template <typename T>
    inline WriteStream& operator<<(const T val) {
        const int32_t val32_t = static_cast<int32_t>(val);
        write(&val32_t, sizeof(int32_t));
        return *this;
    }

    template <typename T>
    inline WriteStream& operator<<(const std::vector<T>& val) {
        *this << val.size();
        for(const auto& t : val) {
            *this << t;
        }
        return *this;
    }

    inline WriteStream& operator<<(const SDL_Rect& val) {
        *this << val.x;
        *this << val.y;
        *this << val.w;
        *this << val.h;
        return *this;
    }

    inline WriteStream& operator<<(const std::string& val) {
        const int32_t size = val.size();
        *this << size;
        write(&val[0], size);
        return *this;
    }

    void writeTile(eTile* const tile);
    void writeBuilding(eBuilding* const b);
    void writeCharacter(eCharacter* const c);
    void writeCharacterAction(eCharacterAction* const ca);
    void writeCharActFunc(eCharacterActionFunction* const caf);
    void writeGodAct(eGodAct* const ga);
    void writeDirectionTimes(eDirectionTimes* const d);
    void writeCity(WorldCity* const c);
    void writeBanner(Banner* const b);
    void writSoldierBanner(SoldierBanner* const b);
    void writeGameEvent(eGameEvent* const e);
    void writeInvasionHandler(eInvasionHandler* const i);

    size_t memPos() const { return mDst.memPos(); }
private:
    WriteTarget mDst;
    std::string mFormat;
};

#endif // WRITE_STREAM_H
