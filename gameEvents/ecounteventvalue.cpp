#include "ecounteventvalue.h"

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
    dst << mCount;
    dst << mMinCount;
    dst << mMaxCount;
}

void eCountEventValue::read(eReadStream& src) {
    src >> mCount;
    src >> mMinCount;
    src >> mMaxCount;
}
