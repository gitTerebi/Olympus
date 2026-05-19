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
#include "characters/actions/walkable/ewalkableobject.h"
#include "characters/actions/walkable/ehasresourceobject.h"

class eGameBoard;
class eBuilding;
class eCharacter;
class eCharacterAction;
class eCharacterActionFunction;
class eTile;
class eSoldierBanner;
class eBanner;
class eWorldBoard;
class eWorldCity;
class eGameEvent;
class eInvasionHandler;
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

    // Subclasses needing a post-load fixup hook call this on the archive
    // instead of reaching for readStream(). No-op while writing.
    template <typename Func>
    void addPostFunc(const Func& func, const char* tag = "?") {
        if(mSrc) mSrc->addPostFunc(func, tag);
    }

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
            std::vector<char> data;
            if(!takeField(name, data)) {
#ifdef SAVE_DEBUG
                printf("[saveLoad] missing field '%s'; using default.\n",
                       name.c_str());
#endif
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
            std::vector<char> data;
            if(!takeField(std::string(name), data)) {
                printf("Invalid save: missing payload '%s'.\n", name);
                return false;
            }
            eReadSource source(const_cast<char*>(data.data()));
            eReadStream src(source);
            src.setFormat(mSrc->format());
            readFunc(src);
            src.transferPostFuncsTo(*mSrc);
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

    bool tileField(const char* const name,
                   eGameBoard& board,
                   eTile*& value) {
        return payloadField(name,
            [this, &value](eWriteStream& dst) {
                dst.writeTile(value);
            },
            [this, &board, &value](eReadStream& src) {
                value = src.readTile(board);
            });
    }

    template <typename Ptr>
    void character(eGameBoard* board, Ptr& value) {
        if(reading()) {
            Ptr* const tgt = &value;
            *tgt = nullptr;
            mSrc->readCharacter(board, [tgt](eCharacter* const c) {
                *tgt = c;
            });
        } else {
            mDst->writeCharacter(value.get());
        }
    }

    template <typename Ptr>
    void building(eGameBoard* board, Ptr& value) {
        if(reading()) {
            Ptr* const tgt = &value;
            *tgt = nullptr;
            mSrc->readBuilding(board, [tgt](eBuilding* const b) {
                *tgt = b;
            });
        } else {
            mDst->writeBuilding(value.get());
        }
    }

    template <typename Ptr>
    bool characterField(const char* const name,
                        eGameBoard* board,
                        Ptr& value) {
        using T = typename std::remove_reference<decltype(*value)>::type;
        Ptr* const tgt = &value;
        return payloadField(
            name,
            [tgt](eWriteStream& dst) { dst.writeCharacter(tgt->get()); },
            [board, tgt](eReadStream& src) {
                *tgt = nullptr;
                src.readCharacter(board, [tgt](eCharacter* const c) {
                    *tgt = static_cast<T*>(c);
                });
            });
    }

    template <typename T>
    bool characterField(const char* const name,
                        eGameBoard* board,
                        T*& value) {
        T** const tgt = &value;
        return payloadField(
            name,
            [tgt](eWriteStream& dst) { dst.writeCharacter(*tgt); },
            [board, tgt](eReadStream& src) {
                *tgt = nullptr;
                src.readCharacter(board, [tgt](eCharacter* const c) {
                    *tgt = static_cast<T*>(c);
                });
            });
    }

    template <typename Ptr>
    bool buildingField(const char* const name,
                       eGameBoard* board,
                       Ptr& value) {
        Ptr* const tgt = &value;
        return payloadField(
            name,
            [tgt](eWriteStream& dst) { dst.writeBuilding(tgt->get()); },
            [board, tgt](eReadStream& src) {
                *tgt = nullptr;
                src.readBuilding(board, [tgt](eBuilding* const b) {
                    *tgt = b;
                });
            });
    }

    template <typename T>
    void buildingAs(eGameBoard* board, stdptr<T>& value) {
        if(reading()) {
            stdptr<T>* const tgt = &value;
            tgt->clear();
            mSrc->readBuilding(board, [tgt](eBuilding* const b) {
                *tgt = static_cast<T*>(b);
            });
        } else {
            mDst->writeBuilding(value.get());
        }
    }

    template <typename T>
    bool buildingAsField(const char* const name,
                         eGameBoard* board,
                         stdptr<T>& value) {
        stdptr<T>* const tgt = &value;
        return payloadField(
            name,
            [tgt](eWriteStream& dst) { dst.writeBuilding(tgt->get()); },
            [board, tgt](eReadStream& src) {
                tgt->clear();
                src.readBuilding(board, [tgt](eBuilding* const b) {
                    *tgt = static_cast<T*>(b);
                });
            });
    }

    template <typename T>
    bool buildingAsField(const char* const name,
                         eGameBoard* board,
                         T*& value) {
        T** const tgt = &value;
        return payloadField(
            name,
            [tgt](eWriteStream& dst) { dst.writeBuilding(*tgt); },
            [board, tgt](eReadStream& src) {
                *tgt = nullptr;
                src.readBuilding(board, [tgt](eBuilding* const b) {
                    *tgt = static_cast<T*>(b);
                });
            });
    }

    template <typename T>
    void characterAs(eGameBoard* board, stdptr<T>& value) {
        if(reading()) {
            stdptr<T>* const tgt = &value;
            tgt->clear();
            mSrc->readCharacter(board, [tgt](eCharacter* const c) {
                *tgt = static_cast<T*>(c);
            });
        } else {
            mDst->writeCharacter(value.get());
        }
    }

    template <typename T>
    bool characterAsField(const char* const name,
                          eGameBoard* board,
                          stdptr<T>& value) {
        stdptr<T>* const tgt = &value;
        return payloadField(
            name,
            [tgt](eWriteStream& dst) { dst.writeCharacter(tgt->get()); },
            [board, tgt](eReadStream& src) {
                tgt->clear();
                src.readCharacter(board, [tgt](eCharacter* const c) {
                    *tgt = static_cast<T*>(c);
                });
            });
    }

    template <typename T>
    void characterAs(eGameBoard* board, T*& value) {
        if(reading()) {
            T** const tgt = &value;
            *tgt = nullptr;
            mSrc->readCharacter(board, [tgt](eCharacter* const c) {
                *tgt = static_cast<T*>(c);
            });
        } else {
            mDst->writeCharacter(value);
        }
    }

    template <typename T>
    bool characterAsField(const char* const name,
                          eGameBoard* board,
                          T*& value) {
        T** const tgt = &value;
        return payloadField(
            name,
            [tgt](eWriteStream& dst) { dst.writeCharacter(*tgt); },
            [board, tgt](eReadStream& src) {
                *tgt = nullptr;
                src.readCharacter(board, [tgt](eCharacter* const c) {
                    *tgt = static_cast<T*>(c);
                });
            });
    }

    void city(eGameBoard* board, stdsptr<eWorldCity>& value) {
        if(reading()) {
            stdsptr<eWorldCity>* const tgt = &value;
            *tgt = nullptr;
            mSrc->readCity(board, [tgt](const stdsptr<eWorldCity>& c) {
                *tgt = c;
            });
        } else {
            mDst->writeCity(value.get());
        }
    }

    void city(eWorldBoard* board, stdsptr<eWorldCity>& value) {
        if(reading()) {
            stdsptr<eWorldCity>* const tgt = &value;
            *tgt = nullptr;
            mSrc->readCity(board, [tgt](const stdsptr<eWorldCity>& c) {
                *tgt = c;
            });
        } else {
            mDst->writeCity(value.get());
        }
    }

    void soldierBanner(eGameBoard* board, stdsptr<eSoldierBanner>& value) {
        if(reading()) {
            stdsptr<eSoldierBanner>* const tgt = &value;
            *tgt = nullptr;
            mSrc->readSoldierBanner(board, [tgt](const stdsptr<eSoldierBanner>& b) {
                *tgt = b;
            });
        } else {
            mDst->writeSoldierBanner(value.get());
        }
    }

    template <typename Ptr>
    bool soldierBannerField(const char* const name,
                            eGameBoard* board,
                            Ptr& value) {
        Ptr* const tgt = &value;
        return payloadField(name,
            [tgt](eWriteStream& dst) { dst.writeSoldierBanner(tgt->get()); },
            [board, tgt](eReadStream& src) {
                *tgt = nullptr;
                src.readSoldierBanner(board, [tgt](const stdsptr<eSoldierBanner>& b) {
                    *tgt = b;
                });
            });
    }

    template <typename T>
    bool bannerField(const char* const name,
                     eGameBoard* board,
                     T*& value) {
        T** const tgt = &value;
        return payloadField(name,
            [tgt](eWriteStream& dst) { dst.writeBanner(*tgt); },
            [board, tgt](eReadStream& src) {
                *tgt = nullptr;
                src.readBanner(board, [tgt](eBanner* const b) {
                    *tgt = static_cast<T*>(b);
                });
            });
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
            T** const tgt = &val;
            *tgt = nullptr;
            mSrc->readGameEvent(board, [tgt](eGameEvent* const e) {
                *tgt = static_cast<T*>(e);
            });
        } else {
            mDst->writeGameEvent(val);
        }
    }

    template <typename T>
    void gameEvent(eGameBoard* board, stdptr<T>& val) {
        if(reading()) {
            stdptr<T>* const tgt = &val;
            tgt->clear();
            mSrc->readGameEvent(board, [tgt](eGameEvent* const e) {
                *tgt = static_cast<T*>(e);
            });
        } else {
            mDst->writeGameEvent(val.get());
        }
    }

    template <typename T>
    bool gameEventField(const char* const name, eGameBoard* board, stdptr<T>& val) {
        stdptr<T>* const tgt = &val;
        return payloadField(name,
            [tgt](eWriteStream& dst) { dst.writeGameEvent(tgt->get()); },
            [board, tgt](eReadStream& src) {
                tgt->clear();
                src.readGameEvent(board, [tgt](eGameEvent* const e) {
                    *tgt = static_cast<T*>(e);
                });
            });
    }

    bool walkableField(const char* const name, stdsptr<eWalkableObject>& val) {
        bool hasValue = val != nullptr;
        const std::string hasName = std::string(name) + ".has";
        this->field(hasName.c_str(), hasValue, false);
        if(!hasValue) {
            if(reading()) val = nullptr;
            return true;
        }
        eWalkableObjectType type = writing() ? val->type() : eWalkableObjectType::ddefault;
        const std::string typeName = std::string(name) + ".type";
        this->field(typeName.c_str(), type);
        if(reading()) val = eWalkableObject::sCreate(type);
        if(!val) {
            printf("[saveLoad] walkableField '%s' unknown type %d.\n",
                   name, static_cast<int>(type));
            return false;
        }
        const bool ok = archiveField(name, [&](eSaveArchive& childAr) {
            val->serialize(childAr);
        });
        if(!ok) printf("[saveLoad] walkableField '%s' missing data.\n", name);
        return ok;
    }

    bool hasResourceField(const char* const name, stdsptr<eHasResourceObject>& val) {
        bool hasValue = val != nullptr;
        const std::string hasName = std::string(name) + ".has";
        this->field(hasName.c_str(), hasValue, false);
        if(!hasValue) {
            if(reading()) val = nullptr;
            return true;
        }
        eHasResourceObjectType type = writing() ? val->type() : eHasResourceObjectType::nonBusy;
        const std::string typeName = std::string(name) + ".type";
        this->field(typeName.c_str(), type);
        if(reading()) val = eHasResourceObject::sCreate(type);
        if(!val) {
            printf("[saveLoad] hasResourceField '%s' unknown type %d.\n",
                   name, static_cast<int>(type));
            return false;
        }
        const bool ok = archiveField(name, [&](eSaveArchive& childAr) {
            val->serialize(childAr);
        });
        if(!ok) printf("[saveLoad] hasResourceField '%s' missing data.\n", name);
        return ok;
    }

    bool directionTimesField(const char* const name,
                             eGameBoard& board,
                             stdsptr<eDirectionTimes>& val) {
        if(reading()) {
            val = std::make_shared<eDirectionTimes>();
            if(!tagged()) {
                const auto ignored = mSrc->readDirectionTimes(board);
                (void)ignored;
                return true;
            }
            std::vector<char> ignored;
            takeField(std::string(name), ignored);
            return true;
        }
        eDirectionTimes empty;
        return payloadField(name,
            [&empty](eWriteStream& dst) { dst.writeDirectionTimes(&empty); },
            [](eReadStream&) {});
    }

    bool godActField(const char* const name,
                     eGameBoard& board,
                     stdsptr<eGodAct>& val) {
        bool hasValue = val != nullptr;
        const std::string hasName = std::string(name) + ".has";
        this->field(hasName.c_str(), hasValue, false);
        if(!hasValue) {
            if(reading()) val = nullptr;
            return true;
        }
        return payloadField(name,
             [&val](eWriteStream& dst) { dst.writeGodAct(val.get()); },
             [&board, &val](eReadStream& src) { val = src.readGodAct(board); });
    }

    template <typename T>
    bool characterActionAsField(const char* const name,
                                eGameBoard* board,
                                stdptr<T>& value) {
        stdptr<T>* const tgt = &value;
        return payloadField(
            name,
            [tgt](eWriteStream& dst) {
                dst.writeCharacterAction(tgt->get());
            },
            [board, tgt](eReadStream& src) {
                tgt->clear();
                src.readCharacterAction(board, [tgt](eCharacterAction* const a) {
                    *tgt = static_cast<T*>(a);
                });
            });
    }

    bool invasionHandlerField(const char* const name,
                              eGameBoard* board,
                              eInvasionHandler*& val) {
        eInvasionHandler** const tgt = &val;
        return payloadField(name,
            [tgt](eWriteStream& dst) { dst.writeInvasionHandler(*tgt); },
            [board, tgt](eReadStream& src) {
                *tgt = nullptr;
                src.readInvasionHandler(board, [tgt](eInvasionHandler* const i) {
                    *tgt = i;
                });
            });
    }

    bool charActFuncField(const char* const name,
                          eGameBoard& board,
                          stdsptr<eCharacterActionFunction>& val) {
        bool hasValue = val != nullptr;
        const std::string hasName = std::string(name) + ".has";
        this->field(hasName.c_str(), hasValue, false);
        if(!hasValue) {
            if(reading()) val = nullptr;
            return true;
        }
        return payloadField(name,
            [&val](eWriteStream& dst) { dst.writeCharActFunc(val.get()); },
            [&board, &val](eReadStream& src) { val = src.readCharActFunc(board); });
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
            auto scratch = std::make_shared<std::decay_t<decltype(values[0])>>();
            const std::string itemName = std::string(name) + "." + std::to_string(i);
            archiveField(itemName.c_str(), [&](eSaveArchive& itemAr) {
                itemFunc(itemAr, *scratch);
                itemAr.addPostFunc([scratch]() {}, "fixedArrayField::scratch");
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
            printf("[saveLoad] invalid field name size %d.\n", nameSize);
            mTaggedEnded = true;
            return false;
        }

        name.clear();
        if(nameSize > 0) {
            name.resize(nameSize);
            mSrc->read(&name[0], nameSize);
        }

        *mSrc >> size;
        if(name.empty() && size == -1) {
            mTaggedEnded = true;
            return false;
        }
        if(size < 0) {
            printf("[saveLoad] invalid field '%s' size %d.\n",
                   name.c_str(), size);
            mTaggedEnded = true;
            return false;
        }
        if(size > maxFieldDataSize) {
            printf("[saveLoad] oversized field '%s' size %d.\n",
                   name.c_str(), size);
            mTaggedEnded = true;
            return false;
        }
        return true;
    }

    bool takeField(const std::string& wanted, std::vector<char>& out) {
        mTaggedTouched = true;
        auto cached = mFields.find(wanted);
        if(cached != mFields.end() && !cached->second.empty()) {
            out = cached->second.front();
            cached->second.erase(cached->second.begin());
            return true;
        }

        while(true) {
            if(mTaggedEnded) return false;
            std::string name;
            int32_t size;
            if(!readFieldHeader(name, size)) return false;
            std::vector<char> data;
            data.resize(size);
            if(size > 0) {
                mSrc->read(data.data(), size);
            }
            if(name == wanted) {
                out = data;
                return true;
            }
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
