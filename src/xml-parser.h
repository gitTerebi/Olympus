#ifndef EXMLPARSER_H
#define EXMLPARSER_H

#include <map>
#include <string>

using eTextGroup = std::map<int, std::string>;
using eTextStrings = std::map<int, eTextGroup>;

struct eMM {
    std::string fTitle;
    std::string fContent;
    std::string fSubtitle;
};

using eMMStrings = std::map<int, eMM>;

namespace eXmlParser {
    bool sParse(eTextStrings& strings,
                const std::string& filePath);
    bool sParse(eMMStrings& strings,
                const std::string& filePath);
};

#endif // EXMLPARSER_H
