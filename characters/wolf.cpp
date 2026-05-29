#include "wolf.h"

#include "actions/ewolfaction.h"
#include "enumbers.h"
#include "engine/etile.h"
#include "textures/egametextures.h"

#include <vector>

Wolf::Wolf(GameBoard& board) :
    eWildAnimal(board, &eCharacterTextures::fWolf,
                eCharacterType::wolf) {
    eGameTextures::loadWolf();
    setCityId(eCityId::neutralAggresive);
}

bool Wolf::takeDamage(const double a, eCharacter* const attacker) {
    const bool dead = eWildAnimal::takeDamage(a, attacker);
    if(dead || !attacker || attacker->dead()) return dead;
    if(attacker->range() <= 0) return dead;
    const auto attackedTile = tile();
    if(!attackedTile) return dead;
    const auto attackerTile = attacker->tile();
    if(!attackerTile) return dead;

    std::vector<eCharacter*> targets;
    constexpr int targetRange = 4;
    for(int x = -targetRange; x <= targetRange; x++) {
        for(int y = -targetRange; y <= targetRange; y++) {
            const auto t = attackerTile->tileRel<eTile>(x, y);
            if(!t) continue;
            const auto chars = t->characters();
            for(const auto& c : chars) {
                if(c->dead()) continue;
                if(c->range() <= 0) continue;
                if(!eTeamIdHelpers::isEnemy(c->teamId(), teamId())) continue;
                targets.push_back(c.get());
            }
        }
    }
    if(targets.empty()) targets.push_back(attacker);

    const int range = eNumbers::sWolfHuntDistance;
    int wolfIndex = 0;
    for(int x = -range; x <= range; x++) {
        for(int y = -range; y <= range; y++) {
            const auto t = attackedTile->tileRel<eTile>(x, y);
            if(!t) continue;
            const auto chars = t->characters();
            for(const auto& c : chars) {
                if(c->type() != eCharacterType::wolf) continue;
                if(c->dead()) continue;
                if(c->teamId() != teamId()) continue;
                const auto action = dynamic_cast<eWolfAction*>(c->action());
                if(!action) continue;
                action->retaliate(targets[wolfIndex % targets.size()]);
                wolfIndex++;
            }
        }
    }
    return dead;
}
