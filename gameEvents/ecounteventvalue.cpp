#include "ecounteventvalue.h"
#include "fileIO/esavearchive.h"

#include "erand.h"
#include "estringhelpers.h"
#include "fileIO/ereadstream.h"
#include "fileIO/ewritestream.h"

void eCountEventValue::chooseCount() {
    if(mMinCount >= mMaxCount) {
        mCount = mMinCount;
    } else {
        const int diff = mMaxCount - mMinCount;
        mCount = mMinCount + (eRand::rand() % diff);
    }
}

void eCountEventValue::longNameReplaceCount(
        const std::string& id, std::string& tmpl) const {
    const auto cStr = std::to_string(mMinCount) + "-" +
                      std::to_string(mMaxCount);
    eStringHelpers::replace(tmpl, id, cStr);
}

void eCountEventValue::serialize(eSaveArchive& ar) {
    ar.field("count", mCount, 16);
    ar.field("minCount", mMinCount, 8);
    ar.field("maxCount", mMaxCount, 16);
}
