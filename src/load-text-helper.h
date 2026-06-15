#ifndef LOAD_TEXT_HELPER_H
#define LOAD_TEXT_HELPER_H

#include <string>
#include <map>

namespace LoadTextHelper {
    using eMap = std::map<std::string, std::string>;
    bool load(const std::string& path, eMap& map);
};

#endif // LOAD_TEXT_HELPER_H
