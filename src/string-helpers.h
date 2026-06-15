#ifndef ESTRINGHELPERS_H
#define ESTRINGHELPERS_H

#include <string>

namespace eStringHelpers {
    bool replace(std::string& source,
                const std::string& from,
                const std::string& to);
    void replaceAll(std::string& source,
                    const std::string& from,
                    const std::string& to);
    std::string pathToName(const std::string& path);

    void replaceSpecial(std::string& value);
};

#endif // ESTRINGHELPERS_H
