#include "eracinghorse.h"

#include "fileIO/save-archive.h"

eRacingHorse::eRacingHorse(GameBoard& board, const int id,
                           const std::vector<PathPoint>& path) :
    Missile(board, MissileType::racingHorse, path),
    mId(id) {
    GameTextures::loadRacingHorses();
}

eRacingHorse::eRacingHorse(GameBoard& board) :
    eRacingHorse(board, 0, {}) {}

std::shared_ptr<Texture>
eRacingHorse::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& ctexs = GameTextures::characters()[sizeId];
    const RacingHorseTextures* texs = nullptr;
    const int id = mId % 4;
    if(id == 0) {
        texs = &ctexs.fRacingHorse1;
    } else if(id == 1) {
        texs = &ctexs.fRacingHorse2;
    } else if(id == 2) {
        texs = &ctexs.fRacingHorse3;
    } else if(id == 3) {
        texs = &ctexs.fRacingHorse4;
    }
    if(!texs) return nullptr;
    const auto& race = texs->fRace;
    const double a = angle();
    const auto& board = Missile::board();
    const auto dir = board.direction();
    double da;
    if(dir == eWorldDirection::N) {
        da = 225;
    } else if(dir == eWorldDirection::E) {
        da = 315;
    } else if(dir == eWorldDirection::S) {
        da = 45;
    } else { // if(dir == eWorldDirection::W) {
        da = 135;
    }
    const int aid = int(std::round((a - da)/22.5) - 1) % race.size();
    const auto& coll = race[aid];
    const int time = 5*textureTime();
    return coll.getTexture(time % coll.size());
}

void eRacingHorse::serializeFields(SaveArchive& ar) {
    Missile::serializeFields(ar);
    ar.field("mId", mId);
}
