#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include <string>

namespace StringHelpers {
    bool replace(std::string& source,
                const std::string& from,
                const std::string& to);
    void replaceAll(std::string& source,
                    const std::string& from,
                    const std::string& to);
    std::string pathToName(const std::string& path);

    void replaceSpecial(std::string& value);
};

#endif // STRING_HELPERS_H
