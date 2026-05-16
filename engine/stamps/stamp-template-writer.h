#ifndef STAMP_TEMPLATE_WRITER_H
#define STAMP_TEMPLATE_WRITER_H

#include "estamptool.h"

#include <string>
#include <vector>

const char* eStampBuildingTypeName(eBuildingType type);
bool eWriteStampTemplate(const std::string& path,
                         const std::vector<eStampElement>& blueprint);

#endif // STAMP_TEMPLATE_WRITER_H
