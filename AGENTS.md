# AGENTS.md

## Answers
ELI5 by default: short, simple, cause -> effect.

## Style
Refactor repeated fns into helpers. Debug log with `printf`. New files: kebab-case, no `e`/`e-` prefix. New classes: PascalCase, no `e` prefix. New include guards: no `E` prefix.

## Build
After making code changes, build with `.\build.bat` to verify. Do not verify with `cmake --build build`.

## Map
Game state: `engine/game-board.*`, `engine/game-board-read.cpp`, `engine/egameboardwrite.cpp`. Cart pathing: `characters/actions/ecarttransporteraction.*`; deliver=`give`, pickup=`take`, max dist=`eCartTransporter::maxDistance()`. Storage/trade orders: `buildings/estoragebuilding.*`, `buildings/trade-post.*`; `setOrders()` maps exports to accept unless explicit get/empty. Text: `text/Zeus_Text.xml` is read-only; reuse runtime strings.

Tile coords (`tile->x()/y()`, `GameBoard::tile(x,y)`) can be negative. Do not bounds-check tile coords with `0..board.width()/height()`: those are d-tile storage bounds. Test tile coords by calling `board.tile(x,y)` and checking null. To scan the whole board, loop d-tile coords with `board.width()/height()`, call `board.dtile(dx,dy)`, then use `tile->x()/y()`.

## Save
Read `fileIO/esavearchive.h` before any save work. Use `field`, `archiveField`, `objectField`, `arrayField`/`dequeField`. No raw bytes (`val()`, `readStream()`, `writeStream()`, raw loops). Field names: stable, unique, descriptive. Enums as raw ints: append-only, pin existing to explicit values. Fix bad writers first; never null-guard the reader to paper over a write bug.
