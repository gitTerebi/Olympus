#include "elanguage.h"

#include "eloadtexthelper.h"

#include "exmlparser.h"
#include "egamedir.h"

eLanguage eLanguage::instance;

const std::string& eLanguage::text(const std::string& key) {
    return instance.fText[key];
}

const std::string& eLanguage::zeusText(const int g, const int s) {
    return instance.fZeusText[g][s];
}

const eMM& eLanguage::zeusMM(const int id) {
    return instance.fZeusMM[id];
}

bool eLanguage::load() {
    return instance.loadImpl();
}

bool eLanguage::loaded() {
    return instance.mLoaded;
}

bool eLanguage::loadImpl() {
    if(mLoaded) return false;
    mLoaded = true;

    eXmlParser::sParse(fZeusText, eGameDir::exeDir() + "../Zeus_Text.xml");
    eXmlParser::sParse(fZeusMM, eGameDir::exeDir() + "../Zeus_MM.xml");

    const std::string path = eGameDir::exeDir() + "../Text/language.txt";
    return eLoadTextHelper::load(path, fText);
}
