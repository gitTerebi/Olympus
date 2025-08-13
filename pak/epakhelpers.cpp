#include "epakhelpers.h"

#include "engine/eresourcetype.h"

eResourceType ePakHelpers::pakResourceByteToType(const uint8_t byte, const bool newVersion) {
    if(byte == 0) {
        return eResourceType::none;
    } else if(byte == 1) {
        return eResourceType::urchin;
    } else if(byte == 2) {
        return eResourceType::fish;
    } else if(byte == 3) {
        return eResourceType::meat;
    } else if(byte == 4) {
        return eResourceType::cheese;
    } else if(byte == 5) {
        return eResourceType::carrots;
    } else if(byte == 6) {
        return eResourceType::onions;
    } else if(byte == 7) {
        return eResourceType::wheat;
    } else if(newVersion && byte == 8) {
        return eResourceType::oranges;
    } else if(byte == 9 + (newVersion ? 0 : -1)) {
        return eResourceType::wood;
    } else if(byte == 10 + (newVersion ? 0 : -1)) {
        return eResourceType::bronze;
    } else if(byte == 11 + (newVersion ? 0 : -1)) {
        return eResourceType::marble;
    } else if(byte == 12 + (newVersion ? 0 : -1)) {
        return eResourceType::grapes;
    } else if(byte == 13 + (newVersion ? 0 : -1)) {
        return eResourceType::olives;
    } else if(byte == 14 + (newVersion ? 0 : -1)) {
        return eResourceType::fleece;
    } else if(!newVersion && byte == 14) {
        return eResourceType::horse;
    } else if(!newVersion && byte == 15) {
        return eResourceType::armor;
    } else if(!newVersion && byte == 16) {
        return eResourceType::sculpture;
    } else if(!newVersion && byte == 17) {
        return eResourceType::oliveOil;
    } else if(!newVersion && byte == 18) {
        return eResourceType::wine;
    } else if(!newVersion && byte == 19) {
        return eResourceType::drachmas;
    } else if(!newVersion && byte == 21) {
        return eResourceType::food;
    } else if(newVersion && byte == 16) {
        return eResourceType::blackMarble;
    } else if(newVersion && byte == 17) {
        return eResourceType::orichalc;
    } else if(newVersion && byte == 18) {
        return eResourceType::armor;
    } else if(newVersion && byte == 19) {
        return eResourceType::sculpture;
    } else if(newVersion && byte == 20) {
        return eResourceType::oliveOil;
    } else if(newVersion && byte == 21) {
        return eResourceType::wine;
    } else if(newVersion && byte == 23) {
        return eResourceType::drachmas;
    }

    printf("Invalid resource byte %i\n", byte);
    return eResourceType::none;
}
