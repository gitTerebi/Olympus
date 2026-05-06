#ifndef ESTAMPBLUEPRINT_H
#define ESTAMPBLUEPRINT_H

#include "estamptool.h"

#include <string>
#include <vector>

bool eReadStampBlueprint(const std::string& path,
                         std::vector<eStampElement>& blueprint);

#endif // ESTAMPBLUEPRINT_H
