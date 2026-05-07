#include "etrailer.h"

#include "textures/egametextures.h"
#include "fileIO/esavearchive.h"

eTrailer::eTrailer(eGameBoard& board) :
    eCharacter(board, eCharacterType::trailer) {
    eGameTextures::loadTrailer();
}

std::shared_ptr<eTexture>
eTrailer::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& charTexs = eGameTextures::characters()[id];
    const int oid = static_cast<int>(rotatedOrientation());
    const eTextureCollection* coll = nullptr;
    const int resCount = mFollow ? mFollow->resCount() : mResCount;
    const auto resType = mFollow ? mFollow->resType() : mResType;
    mResCount = resCount;
    mResType = resType;
    if(resCount <= 0) {
        if(mIsBig) {
            coll = &charTexs.fEmptyBigTrailer;
        } else {
            coll = &charTexs.fEmptyTrailer;
        }
    } else {
        switch(resType) {
        case eResourceType::marble: {
            if(mIsBig) {
                coll = &charTexs.fMarbleBigTrailer;
            } else {
                if(resCount >= 4) {
                    coll = &charTexs.fMarbleTrailer2;
                } else {
                    coll = &charTexs.fMarbleTrailer1;
                }
            }
        } break;
        case eResourceType::blackMarble: {
            if(mIsBig) {
                coll = &charTexs.fBlackMarbleBigTrailer;
            } else {
                if(resCount >= 4) {
                    coll = &charTexs.fBlackMarbleTrailer2;
                } else {
                    coll = &charTexs.fBlackMarbleTrailer1;
                }
            }
        } break;
        case eResourceType::wood: {
            if(resCount >= 8) {
                coll = &charTexs.fWoodTrailer2;
            } else {
                coll = &charTexs.fWoodTrailer1;
            }
        } break;
        case eResourceType::sculpture: {
            coll = &charTexs.fSculptureTrailer;
        } break;
        default: {
            if(mIsBig) {
                coll = &charTexs.fEmptyBigTrailer;
            } else {
                coll = &charTexs.fEmptyTrailer;
            }
        }
        }
    }

    return coll->getTexture(oid);
}

void eTrailer::read(eReadStream& src) {
    eCharacter::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eTrailer::write(eWriteStream& dst) const {
    eCharacter::write(dst);
    eSaveArchive ar(dst);
    const_cast<eTrailer*>(this)->serialize(ar);
}

void eTrailer::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        ar.readStream().readCharacter(&getBoard(), [this](eCharacter* const c) {
            mFollow = static_cast<eCartTransporter*>(c);
        });
    } else {
        ar.writeStream().writeCharacter(mFollow);
    }
    ar.value(mIsBig);
    ar.value(mResCount);
    ar.value(mResType);
}
