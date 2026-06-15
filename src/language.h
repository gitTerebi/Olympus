#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "xml-parser.h"

class Language {
public:
    Language() {}

    static Language instance;

    static bool load();
    static bool loaded();

    static const std::string& text(const std::string& key);

    static const std::string& zeusText(const int g, const int s);
    static const MM& zeusMM(const int id);

    std::map<std::string, std::string> fText;

    std::map<int, std::map<int, std::string>> fZeusText;
    std::map<int, MM> fZeusMM;
private:
    bool loadImpl();
    bool mLoaded = false;
};

#endif // LANGUAGE_H
