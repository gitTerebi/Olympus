#include "engine/egodquest.h"

#include "elanguage.h"

eHeroType eGodQuest::sDefaultHero(
        const GodType gt, const GodQuestId gqi) {
    switch(gt) {
    case GodType::aphrodite: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::hercules;
        case GodQuestId::godQuest2:
            return eHeroType::bellerophon;
        }
    } break;
    case GodType::apollo: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::hercules;
        case GodQuestId::godQuest2:
            return eHeroType::atalanta;
        }
    } break;
    case GodType::ares: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::hercules;
        case GodQuestId::godQuest2:
            return eHeroType::atalanta;
        }
    } break;
    case GodType::artemis: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::hercules;
        case GodQuestId::godQuest2:
            return eHeroType::atalanta;
        }
    } break;
    case GodType::athena: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::perseus;
        case GodQuestId::godQuest2:
            return eHeroType::bellerophon;
        }
    } break;
    case GodType::atlas: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::hercules;
        case GodQuestId::godQuest2:
            return eHeroType::hercules;
        }
    } break;
    case GodType::demeter: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::hercules;
        case GodQuestId::godQuest2:
            return eHeroType::jason;
        }
    } break;
    case GodType::dionysus: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::theseus;
        case GodQuestId::godQuest2:
            return eHeroType::perseus;
        }
    } break;
    case GodType::hades: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::perseus;
        case GodQuestId::godQuest2:
            return eHeroType::theseus;
        }
    } break;
    case GodType::hephaestus: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::achilles;
        case GodQuestId::godQuest2:
            return eHeroType::odysseus;
        }
    } break;
    case GodType::hera: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::bellerophon;
        case GodQuestId::godQuest2:
            return eHeroType::jason;
        }
    } break;
    case GodType::hermes: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::perseus;
        case GodQuestId::godQuest2:
            return eHeroType::achilles;
        }
    } break;
    case GodType::poseidon: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::odysseus;
        case GodQuestId::godQuest2:
            return eHeroType::theseus;
        }
    } break;
    case GodType::zeus: {
        switch(gqi) {
        case GodQuestId::godQuest1:
            return eHeroType::jason;
        case GodQuestId::godQuest2:
            return eHeroType::odysseus;
        }
    } break;
    }
}

std::string eGodQuest::name() const {
    switch(fGod) {
    case GodType::aphrodite: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 6);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 20);
        }
    } break;
    case GodType::apollo: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 3);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 17);
        }
    } break;
    case GodType::ares: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 5);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 19);
        }
    } break;
    case GodType::artemis: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 4);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 18);
        }
    } break;
    case GodType::athena: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 8);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 22);
        }
    } break;
    case GodType::atlas: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 13);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 27);
        }
    } break;
    case GodType::demeter: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 2);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 16);
        }
    } break;
    case GodType::dionysus: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 10);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 24);
        }
    } break;
    case GodType::hades: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 11);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 25);
        }
    } break;
    case GodType::hephaestus: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 9);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 23);
        }
    } break;
    case GodType::hera: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 12);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 26);
        }
    } break;
    case GodType::hermes: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 7);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 21);
        }
    } break;
    case GodType::poseidon: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 1);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 15);
        }
    } break;
    case GodType::zeus: {
        switch(fId) {
        case GodQuestId::godQuest1:
            return eLanguage::zeusText(52, 0);
        case GodQuestId::godQuest2:
            return eLanguage::zeusText(52, 14);
        }
    } break;
    }
    return "";
}
