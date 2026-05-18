#include "eresourcecollectorbase.h"
#include "fileIO/esavearchive.h"

void eResourceCollectorBase::serializeFields(eSaveArchive& ar) {
    eCharacter::serializeFields(ar);
    ar.field("collectedResourceCount", mResCollected, 0);
}
