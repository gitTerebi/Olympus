#include "emonstereventvalue.h"
#include "fileIO/esavearchive.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

void eMonsterEventValue::serialize(eSaveArchive &ar)
{
    ar.field("monster", mMonster);
}
