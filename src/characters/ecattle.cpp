#include "ecattle.h"

#include "iterate-square.h"
#include "engine/game-board.h"
#include "textures/game-textures.h"
#include "numbers.h"
#include "actions/animal-action.h"
#include "fileIO/save-archive.h"

int eCattle::sId = 0;

eCattle::eCattle(GameBoard& board, const eCharacterType type) :
    eCharacter(board, type), mId(sId++) {
    GameTextures::loadCattle();
}

std::shared_ptr<Texture> eCattle::getTexture(const eTileSize size) const {
    const int id = static_cast<int>(size);
    const auto& texs = GameTextures::characters();
    using eCTexs = CattleTextures CharacterTextures::*;
    eCTexs cTexs;
    const auto t = type();
    if(t == eCharacterType::cattle1) {
        cTexs = &CharacterTextures::fCattle1;
    } else if(t == eCharacterType::cattle2) {
        cTexs = &CharacterTextures::fCattle2;
    } else if(t == eCharacterType::cattle3) {
        cTexs = &CharacterTextures::fCattle3;
    } else {
        GameTextures::loadBull();
        const auto& charTexs = texs[id].fBull;
        const TextureCollection* coll = nullptr;
        const int oid = static_cast<int>(rotatedOrientation());
        bool wrap = true;
        const auto a = actionType();
        switch(a) {
        case eCharacterActionType::collect:
        case eCharacterActionType::fight:
        case eCharacterActionType::fight2:
            coll = &charTexs.fAttack[oid];
            break;
        case eCharacterActionType::stand:
        case eCharacterActionType::lay:
            coll = &charTexs.fStand[oid];
            break;
        case eCharacterActionType::carry:
        case eCharacterActionType::walk:
            coll = &charTexs.fWalk[oid];
            break;
        case eCharacterActionType::die:
            wrap = false;
            coll = &charTexs.fDie;
            break;
        default:
            return nullptr;
        }

        return eCharacter::getTexture(coll, wrap, false);
    }
    const auto& charTexs = texs[id].*cTexs;
    const TextureCollection* coll = nullptr;
    const int oid = static_cast<int>(rotatedOrientation());
    bool wrap = true;
    const auto a = actionType();
    switch(a) {
    case eCharacterActionType::collect:
    case eCharacterActionType::fight:
    case eCharacterActionType::stand:
    case eCharacterActionType::lay:
        coll = &charTexs.fStand[oid];
        break;
    case eCharacterActionType::carry:
    case eCharacterActionType::walk:
        coll = &charTexs.fWalk[oid];
        break;
    case eCharacterActionType::die:
        wrap = false;
        coll = &charTexs.fDie;
        break;
    default:
        return nullptr;
    }

    return eCharacter::getTexture(coll, wrap, false);
}

void eCattle::incTime(const int by) {
    eCharacter::incTime(by);
    mMatureWait += by;
    const int matureWait = Numbers::sCattleMaturePeriod;
    if(mMatureWait > matureWait) {
        mMatureWait = 0;
        mature();
    }
}

void eCattle::serializeFields(SaveArchive& ar) {
    eCharacter::serializeFields(ar);
    ar.field("mId", mId);
    ar.field("mMatureWait", mMatureWait);
    if(ar.reading()) {
        const int id = mId;
        ar.addPostFunc([id]() {
            if(id + 1 > sId) sId = id + 1;
        }, "eCattle::sId");
    }
}

bool eCattle::shouldBecomeBull() const {
    const auto t = getSpawnerTile();
    if(!t) return false;
    int nCattle = 0;
    bool hasBull = false;
    for(int k = 0; k < 10; k++) {
        IterateSquare::iterateSquare(k, [&](const int dx, const int dy) {
            const auto tt = t->tileRel<eTile>(dx, dy);
            if(tt) {
                for(const auto c : getBoard().characters()) {
                    if(!c) continue;
                    const auto aa = dynamic_cast<AnimalAction*>(c->action());
                    if(!aa) continue;
                    if(aa->spawnerX() != tt->x() || aa->spawnerY() != tt->y()) continue;
                    const auto ct = c->type();
                    const bool cattle = ct == eCharacterType::cattle1 ||
                                        ct == eCharacterType::cattle2 ||
                                        ct == eCharacterType::cattle3 ||
                                        ct == eCharacterType::bull;
                    if(!cattle) continue;
                    nCattle++;
                    hasBull = ct == eCharacterType::bull;
                    if(hasBull) break;
                }
            }
            if(hasBull) return true;
            return false;
        });
        if(hasBull) break;
    }
    return !hasBull && nCattle > 1;
}

eTile* eCattle::getSpawnerTile() const {
    const auto a = action();
    if(const auto aa = dynamic_cast<AnimalAction*>(a)) {
        const int x = aa->spawnerX();
        const int y = aa->spawnerY();
        return getBoard().tile(x, y);
    }
    return tile();
}

bool eCattle::mature() {
    const auto t = type();
    if(t == eCharacterType::cattle3) {
        return false;
    } else if(t == eCharacterType::cattle2) {
        const bool r = shouldBecomeBull();
        if(r) setType(eCharacterType::bull);
        else setType(eCharacterType::cattle3);
    } else if(t == eCharacterType::cattle1) {
        setType(eCharacterType::cattle2);
    }
    return true;
}
