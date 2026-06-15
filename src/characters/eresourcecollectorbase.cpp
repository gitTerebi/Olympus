#include "eresourcecollectorbase.h"
#include "fileIO/save-archive.h"

void eResourceCollectorBase::serializeFields(SaveArchive& ar) {
    eCharacter::serializeFields(ar);
    ar.field("collectedResourceCount", mResCollected, 0);
}
