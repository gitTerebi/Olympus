#include "fileIO/esavearchive.h"

#include <cstdio>
#include <deque>
#include <string>
#include <vector>

eWriteStream::eWriteStream(const eWriteTarget& dst) :
    mDst(dst) {}

void eWriteStream::writeFormat(const std::string& format) {
    mFormat = format;
    *this << std::string(format);
    *this << 0;
}

eReadStream::eReadStream(const eReadSource& src) :
    mSrc(src) {}

void eReadStream::readFormat() {
    *this >> mFormat;
    int ignoredLegacyVersion;
    *this >> ignoredLegacyVersion;
}

namespace {
int sFailures = 0;

void check(const bool ok, const char* const message) {
    if(ok) return;
    std::printf("FAIL: %s\n", message);
    sFailures++;
}

template <typename WriteFunc, typename ReadFunc>
void roundTrip(const WriteFunc& writeFunc, const ReadFunc& readFunc) {
    std::vector<char> data;
    eWriteTarget target(&data);
    eWriteStream dst(target);
    dst.writeFormat("eZeus.ez2");
    writeFunc(dst);

    eReadSource source(data.data());
    eReadStream src(source);
    src.readFormat();
    readFunc(src);
}

void testArrayFieldKeepsFollowingFieldsAligned() {
    roundTrip(
        [](eWriteStream& dst) {
            eSaveArchive ar(dst);
            std::vector<int> types = {1, 5, 12};
            bool random = true;
            int nextId = 2;
            ar.arrayField("types", types, [](eSaveArchive& ar, int& type) {
                ar.field("t", type);
            });
            ar.field("mRandom", random);
            ar.field("mNextId", nextId);
        },
        [](eReadStream& src) {
            eSaveArchive ar(src);
            std::vector<int> types;
            bool random = false;
            int nextId = -1;
            ar.arrayField("types", types, [](eSaveArchive& ar, int& type) {
                ar.field("t", type);
            });
            ar.field("mRandom", random);
            ar.field("mNextId", nextId);
            check(types.size() == 3, "array count round-trips");
            check(types[0] == 1 && types[1] == 5 && types[2] == 12,
                  "array item values round-trip");
            check(random, "field after array remains aligned");
            check(nextId == 2, "second field after array remains aligned");
        });
}

void testDequeFieldKeepsFollowingFieldsAligned() {
    roundTrip(
        [](eWriteStream& dst) {
            eSaveArchive ar(dst);
            std::deque<int> turns = {4, 8, 15, 16};
            int after = 23;
            ar.dequeField("turns", turns, [](eSaveArchive& ar, int& turn) {
                ar.field("turn", turn);
            });
            ar.field("after", after);
        },
        [](eReadStream& src) {
            eSaveArchive ar(src);
            std::deque<int> turns;
            int after = 0;
            ar.dequeField("turns", turns, [](eSaveArchive& ar, int& turn) {
                ar.field("turn", turn);
            });
            ar.field("after", after);
            check(turns.size() == 4, "deque count round-trips");
            check(turns[0] == 4 && turns[3] == 16, "deque item values round-trip");
            check(after == 23, "field after deque remains aligned");
        });
}

void testFixedArrayFieldConsumesMismatchPayload() {
    roundTrip(
        [](eWriteStream& dst) {
            eSaveArchive ar(dst);
            std::vector<int> values = {7, 8, 9};
            int after = 10;
            ar.arrayField("warnings", values, [](eSaveArchive& ar, int& value) {
                ar.field("value", value);
            });
            ar.field("after", after);
        },
        [](eReadStream& src) {
            eSaveArchive ar(src);
            std::vector<int> values = {0, 0};
            int after = 0;
            const bool ok = ar.fixedArrayField("warnings", values,
                [](eSaveArchive& ar, int& value) {
                    ar.field("value", value);
                });
            ar.field("after", after);
            check(!ok, "fixed array count mismatch is reported");
            check(values[0] == 7 && values[1] == 8,
                  "fixed array reads expected items");
            check(after == 10, "fixed array mismatch does not corrupt next field");
        });
}

void testCountedArrayFieldKeepsFollowingFieldsAligned() {
    roundTrip(
        [](eWriteStream& dst) {
            eSaveArchive ar(dst);
            const std::vector<int> handlers = {3, 1, 4};
            int after = 15;
            ar.countedArrayField("invasionHandlerCount",
                                  static_cast<int>(handlers.size()),
                                  [&handlers](eSaveArchive& ar, const int i) {
                int value = handlers[i];
                ar.field("handler", value);
            });
            ar.field("after", after);
        },
        [](eReadStream& src) {
            eSaveArchive ar(src);
            std::vector<int> handlers;
            ar.countedArrayField("invasionHandlerCount", 0,
                                  [&handlers](eSaveArchive& ar, const int) {
                int value = 0;
                ar.field("handler", value);
                handlers.push_back(value);
            });
            int after = 0;
            ar.field("after", after);
            check(handlers.size() == 3, "counted array count round-trips");
            check(handlers[0] == 3 && handlers[1] == 1 && handlers[2] == 4,
                  "counted array item values round-trip");
            check(after == 15, "field after counted array remains aligned");
        });
}

void testDuplicateFieldNamesReadInWriteOrder() {
    roundTrip(
        [](eWriteStream& dst) {
            eSaveArchive ar(dst);
            int first = 11;
            int second = 22;
            int after = 33;
            ar.field("value", first);
            ar.field("value", second);
            ar.field("after", after);
        },
        [](eReadStream& src) {
            eSaveArchive ar(src);
            int first = 0;
            int second = 0;
            int after = 0;
            ar.field("value", first);
            ar.field("after", after);
            ar.field("value", second);
            check(first == 11 && second == 22,
                  "duplicate field names read in write order");
            check(after == 33, "cached duplicate fields keep later fields aligned");
        });
}
}

int main() {
    testArrayFieldKeepsFollowingFieldsAligned();
    testDequeFieldKeepsFollowingFieldsAligned();
    testFixedArrayFieldConsumesMismatchPayload();
    testCountedArrayFieldKeepsFollowingFieldsAligned();
    testDuplicateFieldNamesReadInWriteOrder();

    if(sFailures == 0) {
        std::printf("PASS: save archive array tests\n");
    }
    return sFailures == 0 ? 0 : 1;
}
