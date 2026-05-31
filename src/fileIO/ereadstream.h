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
class eWorldCity;
class eBanner;
class SoldierBanner;
class eGameEvent;
class eWorldBoard;
class eInvasionHandler;

#include "engine/edirectionlastusetime.h"

class eReadSource {
public:
    eReadSource(std::ifstream* const file) :
        fFile(file) {}
    eReadSource(void* mem) :
        fMem(mem) {}

    inline size_t read(void* const data, const size_t len) {
        assert(fFile || fMem);
        if(fFile) {
            fFile->read(static_cast<char*>(data), len);
            return len;
        } else if(fMem) {
            std::memcpy(data, static_cast<char*>(fMem) + fMemPos, len);
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
};

#define EREAD_TAG(name) (name "@" __FILE__ ":" EREAD_STRINGIFY(__LINE__))
#define EREAD_STRINGIFY(x) EREAD_STRINGIFY2(x)
#define EREAD_STRINGIFY2(x) #x

class eReadStream {
public:
    eReadStream(const eReadSource& src);

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

    inline eReadStream& operator>>(bool& val) {
        read(&val, sizeof(bool));
        return *this;
    }

    inline eReadStream& operator>>(unsigned char& val) {
        read(&val, sizeof(unsigned char));
        return *this;
    }

    inline eReadStream& operator>>(char& val) {
        read(&val, sizeof(char));
        return *this;
    }

    inline eReadStream& operator>>(float& val) {
        read(&val, sizeof(float));
        return *this;
    }

    inline eReadStream& operator>>(double& val) {
        read(&val, sizeof(double));
        return *this;
    }

    inline eReadStream& operator>>(int32_t& val) {
        read(&val, sizeof(int32_t));
        return *this;
    }

    template <typename T>
    inline eReadStream& operator>>(T& val) {
        int32_t val32_t;
        read(&val32_t, sizeof(int32_t));
        val = static_cast<T>(val32_t);
        return *this;
    }

    template <typename T>
    inline eReadStream& operator>>(std::vector<T>& val) {
        int size;
        *this >> size;
        for(int i = 0; i < size; i++) {
            T& t = val.emplace_back();
            *this >> t;
        }
        return *this;
    }

    inline eReadStream& operator>>(SDL_Rect& val) {
        *this >> val.x;
        *this >> val.y;
        *this >> val.w;
        *this >> val.h;
        return *this;
    }

    inline eReadStream& operator>>(std::string& val) {
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
    using eCityFunc = std::function<void(stdsptr<eWorldCity>)>;
    void readCity(GameBoard* board, const eCityFunc& func);
    void readCity(eWorldBoard* board, const eCityFunc& func);
    using eBannerFunc = std::function<void(eBanner*)>;
    void readBanner(GameBoard* board, const eBannerFunc& func);
    using SoldierBannerFunc = std::function<void(stdsptr<SoldierBanner>)>;
    void readSoldierBanner(GameBoard* board, const SoldierBannerFunc& func);
    using eEventFunc = std::function<void(eGameEvent*)>;
    void readGameEvent(GameBoard* board, const eEventFunc& func);
    using eeInvasionHandlerFunc = std::function<void(eInvasionHandler*)>;
    void readInvasionHandler(GameBoard* board, const eeInvasionHandlerFunc& func);

    using eFunc = std::function<void()>;
    void addPostFunc(const eFunc& func, const char* tag = "?");
    void transferPostFuncsTo(eReadStream& dst);
    void handlePostFuncs();

    const std::string& format() const { return mFormat; }
    void setFormat(const std::string& format) { mFormat = format; }
private:
    std::vector<std::pair<eFunc, const char*>> mPostFuncs;

    eReadSource mSrc;

    std::string mFormat;
};

#endif // EREADSTREAM_H
