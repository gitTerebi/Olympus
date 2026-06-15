#ifndef ENG_PARSER_H
#define ENG_PARSER_H

#include "xml-parser.h"

// Reads Zeus native binary text files (.eng) directly, without the
// external engconverter XML step. Mirrors XmlParser's public shape so
// elanguage can try .eng first and fall back to .xml.
namespace EngParser {
    bool sParse(eTextStrings& strings,
                const std::string& filePath);
    bool sParse(eMMStrings& strings,
                const std::string& filePath);
};

#endif // ENG_PARSER_H
