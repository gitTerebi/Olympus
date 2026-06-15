#include "emonstereventvalue.h"
#include "fileIO/save-archive.h"

#include "fileIO/write-stream.h"
#include "fileIO/read-stream.h"

void eMonsterEventValue::serialize(SaveArchive &ar)
{
    ar.field("monster", mMonster);
}
