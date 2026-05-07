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

void eCountEventValue::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eCountEventValue*>(this)->serialize(ar);
}

void eCountEventValue::serialize(eSaveArchive& ar) {
    ar.field("mCount", mCount);
    ar.field("mMinCount", mMinCount);
    ar.field("mMaxCount", mMaxCount);
}

void eCountEventValue::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}
