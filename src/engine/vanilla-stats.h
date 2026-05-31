#ifndef VANILLA_STATS_H
#define VANILLA_STATS_H

class eCharacterBase;
class GameBoard;
enum class Difficulty;
enum class eCharacterType;

namespace VanillaStats {

const char* figureName(eCharacterType type);

void apply(eCharacterBase& c, Difficulty d, bool isPlayer = false);

void applyForCity(eCharacterBase& c, GameBoard& board);

}

#endif
