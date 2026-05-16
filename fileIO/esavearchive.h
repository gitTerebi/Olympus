#ifndef ESAVEARCHIVE_H
#define ESAVEARCHIVE_H

#include <functional>
#include <map>
#include <memory>
#include <cstdio>
#include <algorithm>
#include <deque>
#include <type_traits>
#include <vector>
#include <cstdint>
#include <string>

#include "estreams.h"

class eGameBoard;
class eBuilding;
class eCharacter;
class eCharacterAction;
class eTile;
class eSoldierBanner;
class eWorldBoard;
class eWorldCity;
class eGameEvent;
enum class eCharActionType;

class eSaveArchive {
public:
    explicit eSaveArchive(eReadStream& src) : mSrc(&src) {}
    explicit eSaveArchive(eWriteStream& dst) : mDst(&dst) {}
    ~eSaveArchive() {
        if(mTaggedTouched && writing()) {
            *mDst << std::string();
            *mDst << int32_t(-1);
        } else if(mTaggedTouched && reading() && !mTaggedEnded) {
            skipRemainingFields();
        }
    }

    bool reading() const { return mSrc; }
    bool writing() const { return mDst; }

    eReadStream& readStream() const { return *mSrc; }

    eWriteStream& writeStream() const { return *mDst; }

    // Use only in SAVE_COMPAT_LEGACY_FALLBACK branches after tagged payload lookup fails.
    eReadStream& legacyReadStream() const { return *mSrc; }

    template <typename T>
    void value(T& value) {
        rawValue(value);
    }

    template <typename T>
    bool field(const char* const name, T& value) {
        return this->field(std::string(name), value);
    }

    template <typename T>
    bool field(const char* const name, T& value, const T def) {
        if(!reading()) return this->field(std::string(name), value);
        value = def;
        const bool found = this->field(std::string(name), value);
        if(!found) value = def;
        return found;
    }

    template <typename T>
    bool field(const std::string& name, T& value) {
        if(!tagged()) {
            this->rawValue(value);
            return true;
        }

        mTaggedTouched = true;
        if(writing()) {
            mFieldBuffer.clear();
            eWriteTarget target(&mFieldBuffer);
            eWriteStream tmp(target);
            tmp << value;

            *mDst << name;
            *mDst << static_cast<int32_t>(mFieldBuffer.size());
            mDst->write(mFieldBuffer.data(), mFieldBuffer.size());
            return true;
        } else {
            auto data = takeField(name);
            if(data.empty()) {
                if constexpr(std::is_default_constructible_v<T> &&
                             std::is_assignable_v<T&, T>) {
                    value = T{};
                }
                return false;
            }
            eReadSource source(const_cast<char*>(data.data()));
            eReadStream src(source);
            src >> value;
            return true;
        }
    }

    // Serialize an object that has read(eReadStream&) / write(eWriteStream&)
    // as a single tagged payload field. The object's bytes are length-prefixed
    // so its internal framing (including its own tagged archive terminator)
    // cannot desync the outer archive.
    template <typename T>
    bool objectField(const char* const name, T& obj) {
        return payloadField(name,
                            [&obj](eWriteStream& dst) { obj.write(dst); },
                            [&obj](eReadStream& src) { obj.read(src); });
    }

    template <typename Func>
    bool archiveField(const char* const name, const Func& func) {
        return payloadField(
            name,
            [&func](eWriteStream& dst) {
                eSaveArchive childAr(dst);
                func(childAr);
            },
            [&func](eReadStream& src) {
                eSaveArchive childAr(src);
                func(childAr);
            });
    }

    template <typename WriteFunc, typename ReadFunc>
    bool payloadField(const char* const name,
                      const WriteFunc& writeFunc,
                      const ReadFunc& readFunc) {
        if(!tagged()) {
            if(reading()) readFunc(*mSrc);
            else writeFunc(*mDst);
            return true;
        }

        mTaggedTouched = true;
        if(writing()) {
            mFieldBuffer.clear();
            eWriteTarget target(&mFieldBuffer);
            eWriteStream tmp(target);
            tmp.setFormat(mDst->format());
            writeFunc(tmp);

            const std::string nameStr(name);
            *mDst << nameStr;
            *mDst << static_cast<int32_t>(mFieldBuffer.size());
            mDst->write(mFieldBuffer.data(), mFieldBuffer.size());
            return true;
        } else {
            auto data = takeField(std::string(name));
            if(data.empty()) {
                printf("Invalid save: missing payload '%s'.\n", name);
                return false;
            }
            eReadSource source(const_cast<char*>(data.data()));
            eReadStream src(source);
            src.setFormat(mSrc->format());
            readFunc(src);
            return true;
        }
    }

    void tile(eTile*& tile, eGameBoard& board) {
        if(reading()) {
            tile = mSrc->readTile(board);
        } else {
            mDst->writeTile(tile);
        }
    }

    template <typename Ptr>
    void character(eGameBoard* board, Ptr& value) {
        if(reading()) {
            value = nullptr;
            mSrc->readCharacter(board, [&value](eCharacter* const c) {
                value = c;
            });
        } else {
            mDst->writeCharacter(value.get());
        }
    }

    template <typename Ptr>
    void building(eGameBoard* board, Ptr& value) {
        if(reading()) {
            value = nullptr;
            mSrc->readBuilding(board, [&value](eBuilding* const b) {
                value = b;
            });
        } else {
            mDst->writeBuilding(value.get());
        }
    }

    template <typename Ptr>
    bool characterField(const char* const name,
                        eGameBoard* board,
                        Ptr& value) {
        return payloadField(
            name,
            [&value](eWriteStream& dst) { dst.writeCharacter(value.get()); },
            [board, &value](eReadStream& src) {
                value = nullptr;
                src.readCharacter(board, [&value](eCharacter* const c) {
                    value = c;
                });
            });
    }

    template <typename Ptr>
    bool buildingField(const char* const name,
                       eGameBoard* board,
                       Ptr& value) {
        return payloadField(
            name,
            [&value](eWriteStream& dst) { dst.writeBuilding(value.get()); },
            [board, &value](eReadStream& src) {
                value = nullptr;
                src.readBuilding(board, [&value](eBuilding* const b) {
                    value = b;
                });
            });
    }

    template <typename T>
    void buildingAs(eGameBoard* board, stdptr<T>& value) {
        if(reading()) {
            value.clear();
            mSrc->readBuilding(board, [&value](eBuilding* const b) {
                value = static_cast<T*>(b);
            });
        } else {
            mDst->writeBuilding(value.get());
        }
    }

    template <typename T>
    bool buildingAsField(const char* const name,
                         eGameBoard* board,
                         stdptr<T>& value) {
        return payloadField(
            name,
            [&value](eWriteStream& dst) { dst.writeBuilding(value.get()); },
            [board, &value](eReadStream& src) {
                value.clear();
                src.readBuilding(board, [&value](eBuilding* const b) {
                    value = static_cast<T*>(b);
                });
            });
    }

    template <typename T>
    bool buildingAsField(const char* const name,
                         eGameBoard* board,
                         T*& value) {
        return payloadField(
            name,
            [&value](eWriteStream& dst) { dst.writeBuilding(value); },
            [board, &value](eReadStream& src) {
                value = nullptr;
                src.readBuilding(board, [&value](eBuilding* const b) {
                    value = static_cast<T*>(b);
                });
            });
    }

    template <typename T>
    void characterAs(eGameBoard* board, stdptr<T>& value) {
        if(reading()) {
            value.clear();
            mSrc->readCharacter(board, [&value](eCharacter* const c) {
                value = static_cast<T*>(c);
            });
        } else {
            mDst->writeCharacter(value.get());
        }
    }

    template <typename T>
    bool characterAsField(const char* const name,
                          eGameBoard* board,
                          stdptr<T>& value) {
        return payloadField(
            name,
            [&value](eWriteStream& dst) { dst.writeCharacter(value.get()); },
            [board, &value](eReadStream& src) {
                value.clear();
                src.readCharacter(board, [&value](eCharacter* const c) {
                    value = static_cast<T*>(c);
                });
            });
    }

    template <typename T>
    void characterAs(eGameBoard* board, T*& value) {
        if(reading()) {
            value = nullptr;
            mSrc->readCharacter(board, [&value](eCharacter* const c) {
                value = static_cast<T*>(c);
            });
        } else {
            mDst->writeCharacter(value);
        }
    }

    template <typename T>
    bool characterAsField(const char* const name,
                          eGameBoard* board,
                          T*& value) {
        return payloadField(
            name,
            [&value](eWriteStream& dst) { dst.writeCharacter(value); },
            [board, &value](eReadStream& src) {
                value = nullptr;
                src.readCharacter(board, [&value](eCharacter* const c) {
                    value = static_cast<T*>(c);
                });
            });
    }

    void city(eGameBoard* board, stdsptr<eWorldCity>& value) {
        if(reading()) {
            value = nullptr;
            mSrc->readCity(board, [&value](const stdsptr<eWorldCity>& c) {
                value = c;
            });
        } else {
            mDst->writeCity(value.get());
        }
    }

    void city(eWorldBoard* board, stdsptr<eWorldCity>& value) {
        if(reading()) {
            value = nullptr;
            mSrc->readCity(board, [&value](const stdsptr<eWorldCity>& c) {
                value = c;
            });
        } else {
            mDst->writeCity(value.get());
        }
    }

    void soldierBanner(eGameBoard* board, stdsptr<eSoldierBanner>& value) {
        if(reading()) {
            value = nullptr;
            mSrc->readSoldierBanner(board, [&value](const stdsptr<eSoldierBanner>& b) {
                value = b;
            });
        } else {
            mDst->writeSoldierBanner(value.get());
        }
    }

    template <typename T>
    void object(T& value) {
        if(reading()) value.read(*mSrc);
        else value.write(*mDst);
    }

    template <typename T>
    void object(std::shared_ptr<T>& value) {
        if(reading()) value->read(*mSrc);
        else value->write(*mDst);
    }

    template <typename T>
    void gameEvent(eGameBoard* board, T*& val) {
        if(reading()) {
            val = nullptr;
            mSrc->readGameEvent(board, [&val](eGameEvent* const e) {
                val = static_cast<T*>(e);
            });
        } else {
            mDst->writeGameEvent(val);
        }
    }

    template <typename T>
    void gameEvent(eGameBoard* board, stdptr<T>& val) {
        if(reading()) {
            val.clear();
            mSrc->readGameEvent(board, [&val](eGameEvent* const e) {
                val = static_cast<T*>(e);
            });
        } else {
            mDst->writeGameEvent(val.get());
        }
    }

    // Saved arrays must use these helpers. Raw stream loops are legacy-only.
    template <typename T, typename Func>
    bool arrayField(const char* const name,
                    std::vector<T>& values,
                    const Func& itemFunc) {
        int count = static_cast<int>(values.size());
        const std::string countName = std::string(name) + ".count";
        this->field(countName, count);
        if(count < 0) {
            printf("Invalid save: array '%s' bad count %d.\n", name, count);
            return false;
        }
        if(reading()) values.resize(count);
        for(int i = 0; i < count; i++) {
            const std::string itemName = std::string(name) + "." + std::to_string(i);
            archiveField(itemName.c_str(), [&](eSaveArchive& itemAr) {
                itemFunc(itemAr, values[i]);
            });
        }
        return true;
    }

    template <typename T, typename Func>
    bool dequeField(const char* const name,
                    std::deque<T>& values,
                    const Func& itemFunc) {
        int count = static_cast<int>(values.size());
        const std::string countName = std::string(name) + ".count";
        this->field(countName, count);
        if(count < 0) {
            printf("Invalid save: deque '%s' bad count %d.\n", name, count);
            return false;
        }
        if(reading()) values.resize(count);
        for(int i = 0; i < count; i++) {
            const std::string itemName = std::string(name) + "." + std::to_string(i);
            archiveField(itemName.c_str(), [&](eSaveArchive& itemAr) {
                itemFunc(itemAr, values[i]);
            });
        }
        return true;
    }

    template <typename Func>
    bool countedArrayField(const char* const name,
                           const int writeCount,
                           const Func& itemFunc) {
        int count = writeCount;
        const std::string countName = std::string(name) + ".count";
        this->field(countName, count);
        if(count < 0) {
            printf("Invalid save: counted array '%s' bad count %d.\n", name, count);
            return false;
        }
        for(int i = 0; i < count; i++) {
            const std::string itemName = std::string(name) + "." + std::to_string(i);
            archiveField(itemName.c_str(), [&](eSaveArchive& itemAr) {
                itemFunc(itemAr, i);
            });
        }
        return true;
    }

    template <typename T, typename Func>
    bool fixedArrayField(const char* const name,
                         std::vector<T>& values,
                         const Func& itemFunc) {
        const int expected = static_cast<int>(values.size());
        int count = expected;
        const std::string countName = std::string(name) + ".count";
        this->field(countName, count);
        if(count < 0) {
            printf("Invalid save: fixed array '%s' bad count %d.\n", name, count);
            return false;
        }
        if(count != expected) {
            printf("Invalid save: fixed array '%s' count mismatch expected=%d saved=%d.\n",
                   name, expected, count);
        }
        const int readCount = std::min(count, expected);
        for(int i = 0; i < readCount; i++) {
            const std::string itemName = std::string(name) + "." + std::to_string(i);
            archiveField(itemName.c_str(), [&](eSaveArchive& itemAr) {
                itemFunc(itemAr, values[i]);
            });
        }
        for(int i = readCount; i < count; i++) {
            std::decay_t<decltype(values[0])> scratch;
            const std::string itemName = std::string(name) + "." + std::to_string(i);
            archiveField(itemName.c_str(), [&](eSaveArchive& itemAr) {
                itemFunc(itemAr, scratch);
            });
        }
        return count == expected;
    }

    template <typename T>
    void characterAction(std::shared_ptr<T>& action, const std::function<std::shared_ptr<T>(eCharActionType)>& create) {
        bool hasAction = action != nullptr;
        this->rawValue(hasAction);
        if(!hasAction) {
            if(reading()) action = nullptr;
            return;
        }

        if(reading()) {
            eCharActionType type;
            this->rawValue(type);
            action = create(type);
            action->read(*mSrc);
        } else {
            eCharActionType type = action->type();
            this->rawValue(type);
            action->write(*mDst);
        }
    }

private:
    template <typename T>
    void rawValue(T& value) {
        if(reading()) {
            *mSrc >> value;
        } else {
            *mDst << value;
        }
    }

    static constexpr int32_t maxFieldNameSize = 256;
    static constexpr int32_t maxFieldDataSize = 64 * 1024 * 1024;

    bool tagged() const {
        if(writing()) return mDst->format() == "eZeus.ez2";
        return mSrc->format() == "eZeus.ez2";
    }

    bool readFieldHeader(std::string& name, int32_t& size) {
        int32_t nameSize = 0;
        *mSrc >> nameSize;
        if(nameSize < 0 || nameSize > maxFieldNameSize) {
            mTaggedEnded = true;
            return false;
        }

        name.clear();
        if(nameSize > 0) {
            name.resize(nameSize);
            mSrc->read(&name[0], nameSize);
        }

        *mSrc >> size;
        if(size < 0) {
            mTaggedEnded = true;
            return false;
        }
        if(size > maxFieldDataSize) {
            mTaggedEnded = true;
            return false;
        }
        return true;
    }

    std::vector<char> takeField(const std::string& wanted) {
        mTaggedTouched = true;
        auto cached = mFields.find(wanted);
        if(cached != mFields.end() && !cached->second.empty()) {
            auto data = cached->second.front();
            cached->second.erase(cached->second.begin());
            return data;
        }

        while(true) {
            if(mTaggedEnded) return {};
            std::string name;
            int32_t size;
            if(!readFieldHeader(name, size)) return {};
            std::vector<char> data;
            data.resize(size);
            if(size > 0) {
                mSrc->read(data.data(), size);
            }
            if(name == wanted) return data;
            mFields[name].push_back(data);
        }
    }

    bool readField(std::string& name, std::vector<char>& data) {
        if(mTaggedEnded) return false;
        int32_t size;
        if(!readFieldHeader(name, size)) return false;
        data.resize(size);
        if(size > 0) {
            mSrc->read(data.data(), size);
        }
        return true;
    }

    void skipRemainingFields() {
        while(!mTaggedEnded) {
            std::string name;
            int32_t size;
            if(!readFieldHeader(name, size)) return;
            mSrc->skip(size);
        }
    }

    eReadStream* mSrc = nullptr;
    eWriteStream* mDst = nullptr;
    bool mTaggedTouched = false;
    bool mTaggedEnded = false;
    std::map<std::string, std::vector<std::vector<char>>> mFields;
    std::vector<char> mFieldBuffer;
};

#endif // ESAVEARCHIVE_H
