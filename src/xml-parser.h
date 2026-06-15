#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <map>
#include <string>

using eTextGroup = std::map<int, std::string>;
using eTextStrings = std::map<int, eTextGroup>;

struct MM {
    std::string fTitle;
    std::string fContent;
    std::string fSubtitle;
};

using eMMStrings = std::map<int, MM>;

namespace XmlParser {
    bool sParse(eTextStrings& strings,
                const std::string& filePath);
    bool sParse(eMMStrings& strings,
                const std::string& filePath);
};

#endif // XML_PARSER_H
