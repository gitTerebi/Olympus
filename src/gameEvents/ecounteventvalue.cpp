#include "ecounteventvalue.h"
#include "fileIO/save-archive.h"

#include "rand.h"
#include "string-helpers.h"
#include "fileIO/read-stream.h"
#include "fileIO/write-stream.h"

void eCountEventValue::chooseCount() {
    if(mMinCount >= mMaxCount) {
        mCount = mMinCount;
    } else {
        const int diff = mMaxCount - mMinCount;
        mCount = mMinCount + (Rand::rand() % diff);
    }
}

void eCountEventValue::longNameReplaceCount(
        const std::string& id, std::string& tmpl) const {
    const auto cStr = std::to_string(mMinCount) + "-" +
                      std::to_string(mMaxCount);
    StringHelpers::replace(tmpl, id, cStr);
}

void eCountEventValue::serialize(SaveArchive& ar) {
    ar.field("count", mCount, 16);
    ar.field("minCount", mMinCount, 8);
    ar.field("maxCount", mMaxCount, 16);
}
