#include "egodquest.h"

#include "elanguage.h"

eHeroType eGodQuest::sDefaultHero(
        const eGodType gt, const eGodQuestId gqi) {
    switch(gt) {
    case eGodType::aphrodite: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::hercules;
        case eGodQuestId::godQuest2:
            return eHeroType::bellerophon;
        }
    } break;
    case eGodType::apollo: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::hercules;
        case eGodQuestId::godQuest2:
            return eHeroType::atalanta;
        }
    } break;
    case eGodType::ares: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::hercules;
        case eGodQuestId::godQuest2:
            return eHeroType::atalanta;
        }
    } break;
    case eGodType::artemis: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::hercules;
        case eGodQuestId::godQuest2:
            return eHeroType::atalanta;
        }
    } break;
    case eGodType::athena: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::perseus;
        case eGodQuestId::godQuest2:
            return eHeroType::bellerophon;
        }
    } break;
    case eGodType::atlas: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::hercules;
        case eGodQuestId::godQuest2:
            return eHeroType::hercules;
        }
    } break;
    case eGodType::demeter: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::hercules;
        case eGodQuestId::godQuest2:
            return eHeroType::jason;
        }
    } break;
    case eGodType::dionysus: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::theseus;
        case eGodQuestId::godQuest2:
            return eHeroType::perseus;
        }
    } break;
    case eGodType::hades: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::perseus;
        case eGodQuestId::godQuest2:
            return eHeroType::theseus;
        }
    } break;
    case eGodType::hephaestus: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::achilles;
        case eGodQuestId::godQuest2:
            return eHeroType::odysseus;
        }
    } break;
    case eGodType::hera: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::bellerophon;
        case eGodQuestId::godQuest2:
            return eHeroType::jason;
        }
    } break;
    case eGodType::hermes: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::perseus;
        case eGodQuestId::godQuest2:
            return eHeroType::achilles;
        }
    } break;
    case eGodType::poseidon: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::odysseus;
        case eGodQuestId::godQuest2:
            return eHeroType::theseus;
        }
    } break;
    case eGodType::zeus: {
        switch(gqi) {
        case eGodQuestId::godQuest1:
            return eHeroType::jason;
        case eGodQuestId::godQuest2:
            return eHeroType::odysseus;
        }
    } break;
    }
}

std::string eGodQuest::name() const {
    switch(fGod) {
    case eGodType::aphrodite: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 6);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 20);
        }
    } break;
    case eGodType::apollo: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 3);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 17);
        }
    } break;
    case eGodType::ares: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 5);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 19);
        }
    } break;
    case eGodType::artemis: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 4);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 18);
        }
    } break;
    case eGodType::athena: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 8);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 22);
        }
    } break;
    case eGodType::atlas: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 13);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 27);
        }
    } break;
    case eGodType::demeter: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 2);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 16);
        }
    } break;
    case eGodType::dionysus: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 10);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 24);
        }
    } break;
    case eGodType::hades: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 11);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 25);
        }
    } break;
    case eGodType::hephaestus: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 9);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 23);
        }
    } break;
    case eGodType::hera: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 12);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 26);
        }
    } break;
    case eGodType::hermes: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 7);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 21);
        }
    } break;
    case eGodType::poseidon: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 1);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 15);
        }
    } break;
    case eGodType::zeus: {
        switch(fId) {
        case eGodQuestId::godQuest1:
            return eLanguage::zeusText(52, 0);
        case eGodQuestId::godQuest2:
            return eLanguage::zeusText(52, 14);
        }
    } break;
    }
    return "";
}
