#include "language.h"

#include "load-text-helper.h"

#include "xml-parser.h"
#include "eng-parser.h"
#include "game-dir.h"

Language Language::instance;

const std::string& Language::text(const std::string& key) {
    return instance.fText[key];
}

const std::string& Language::zeusText(const int g, const int s) {
    return instance.fZeusText[g][s];
}

const MM& Language::zeusMM(const int id) {
    return instance.fZeusMM[id];
}

bool Language::load() {
    return instance.loadImpl();
}

bool Language::loaded() {
    return instance.mLoaded;
}

bool Language::loadImpl() {
    if(mLoaded) return false;
    mLoaded = true;

    if(!EngParser::sParse(fZeusText, GameDir::path("Zeus_Text.eng")))
        XmlParser::sParse(fZeusText, GameDir::exeDir() + "../Zeus_Text.xml");
    if(!EngParser::sParse(fZeusMM, GameDir::path("Zeus_MM.eng")))
        XmlParser::sParse(fZeusMM, GameDir::exeDir() + "../Zeus_MM.xml");

    const std::string path = GameDir::exeDir() + "../Text/language.txt";
    return LoadTextHelper::load(path, fText);
}
