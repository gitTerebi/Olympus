#ifndef EREADSTREAM_H
#define EREADSTREAM_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <string>
#include <functional>
#include <map>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <vector>

#include "pointers/estdpointer.h"

class eTile;
class eBuilding;
class eCharacter;
class eCharacterAction;
class GameBoard;
class eCharacterActionFunction;
class eGodAct;
class WorldCity;
class Banner;
class SoldierBanner;
class eGameEvent;
class WorldBoard;
class eInvasionHandler;

#include "engine/edirectionlastusetime.h"

class ReadSource {
public:
    ReadSource(std::ifstream* const file) :
        fFile(file) {}
    ReadSource(void* mem, size_t size) :
        fMem(mem), fMemSize(size) {}
    ReadSource(void* mem) :
        fMem(mem), fMemSize(SIZE_MAX) {}

    inline size_t read(void* const data, const size_t len) {
        assert(fFile || fMem);
        if(fFile) {
            fFile->read(static_cast<char*>(data), len);
            return len;
        } else if(fMem) {
            const size_t avail = (fMemPos < fMemSize) ? (fMemSize - fMemPos) : 0;
            const size_t actual = len < avail ? len : avail;
            if(actual > 0) std::memcpy(data, static_cast<char*>(fMem) + fMemPos, actual);
            if(actual < len) std::memset(static_cast<char*>(data) + actual, 0, len - actual);
            fMemPos += len;
            return len;
        }
        return 0;
    }

    inline int64_t pos() const {
        if(fFile) return fFile->tellg();
        return static_cast<int64_t>(fMemPos);
    }

    inline void seek(const int64_t pos) {
        if(fFile) {
            fFile->seekg(pos);
        } else if(fMem) {
            fMemPos = static_cast<size_t>(pos);
        }
    }
private:
    std::ifstream* fFile = nullptr;
    void* fMem = nullptr;
    size_t fMemPos = 0;
    size_t fMemSize = SIZE_MAX;
};

#define EREAD_TAG(name) (name "@" __FILE__ ":" EREAD_STRINGIFY(__LINE__))
#define EREAD_STRINGIFY(x) EREAD_STRINGIFY2(x)
#define EREAD_STRINGIFY2(x) #x

class ReadStream {
public:
    ReadStream(const ReadSource& src);

    void readFormat();

    inline size_t read(void* const data, const size_t len) {
        return mSrc.read(data, len);
    }

    int64_t pos() const { return mSrc.pos(); }
    void seek(const int64_t pos) { mSrc.seek(pos); }

    void skip(const size_t len) {
        std::vector<char> buffer(len);
        if(len > 0) read(buffer.data(), len);
    }

    inline ReadStream& operator>>(bool& val) {
        read(&val, sizeof(bool));
        return *this;
    }

    inline ReadStream& operator>>(unsigned char& val) {
        read(&val, sizeof(unsigned char));
        return *this;
    }

    inline ReadStream& operator>>(char& val) {
        read(&val, sizeof(char));
        return *this;
    }

    inline ReadStream& operator>>(float& val) {
        read(&val, sizeof(float));
        return *this;
    }

    inline ReadStream& operator>>(double& val) {
        read(&val, sizeof(double));
        return *this;
    }

    inline ReadStream& operator>>(int32_t& val) {
        read(&val, sizeof(int32_t));
        return *this;
    }

    template <typename T>
    inline ReadStream& operator>>(T& val) {
        int32_t val32_t;
        read(&val32_t, sizeof(int32_t));
        val = static_cast<T>(val32_t);
        return *this;
    }

    template <typename T>
    inline ReadStream& operator>>(std::vector<T>& val) {
        int size;
        *this >> size;
        for(int i = 0; i < size; i++) {
            T& t = val.emplace_back();
            *this >> t;
        }
        return *this;
    }

    inline ReadStream& operator>>(SDL_Rect& val) {
        *this >> val.x;
        *this >> val.y;
        *this >> val.w;
        *this >> val.h;
        return *this;
    }

    inline ReadStream& operator>>(std::string& val) {
        int32_t size;
        *this >> size;
        if(size == 0) {
            val = "";
        } else {
            val.resize(size);
            read(&val[0], size);
        }
        return *this;
    }

    eTile* readTile(GameBoard& board);
    using eBuildingFunc = std::function<void(eBuilding*)>;
    void readBuilding(GameBoard* board,
                      const eBuildingFunc& func,
                      const char* tag = "building");
    using eCharFunc = std::function<void(eCharacter*)>;
    void readCharacter(GameBoard* board,
                       const eCharFunc& func,
                       const char* tag = "character");
    using eCharActFunc = std::function<void(eCharacterAction*)>;
    void readCharacterAction(GameBoard* board,
                             const eCharActFunc& func,
                             const char* tag = "characterAction");
    stdsptr<eCharacterActionFunction> readCharActFunc(
            GameBoard& board);
    stdsptr<eGodAct> readGodAct(GameBoard& board);
    stdsptr<eDirectionTimes> readDirectionTimes(
            GameBoard& board);
    using eCityFunc = std::function<void(stdsptr<WorldCity>)>;
    void readCity(GameBoard* board, const eCityFunc& func);
    void readCity(WorldBoard* board, const eCityFunc& func);
    using eBannerFunc = std::function<void(Banner*)>;
    void readBanner(GameBoard* board, const eBannerFunc& func);
    using SoldierBannerFunc = std::function<void(stdsptr<SoldierBanner>)>;
    void readSoldierBanner(GameBoard* board, const SoldierBannerFunc& func);
    using eEventFunc = std::function<void(eGameEvent*)>;
    void readGameEvent(GameBoard* board, const eEventFunc& func);
    using eeInvasionHandlerFunc = std::function<void(eInvasionHandler*)>;
    void readInvasionHandler(GameBoard* board, const eeInvasionHandlerFunc& func);

    using eFunc = std::function<void()>;
    void addPostFunc(const eFunc& func, const char* tag = "?");
    void transferPostFuncsTo(ReadStream& dst);
    void handlePostFuncs();

    const std::string& format() const { return mFormat; }
    void setFormat(const std::string& format) { mFormat = format; }
private:
    std::vector<std::pair<eFunc, const char*>> mPostFuncs;

    ReadSource mSrc;

    std::string mFormat;
};

#endif // EREADSTREAM_H
