# AGENTS.md

## Code Style

Refactor repeated functions into shared local helpers where possible.
Debug log using printf, not std out.
New files: kebab-case, no `e` or `e-` prefix (e.g. `storage-widget.cpp` not `estorage-widget.cpp` or `e-storage-widget.cpp`).

## JSON Saves
G:\games\eZeus\eZeus-0.8.2-beta\Save\Tim

## C++ Style

Always mark virtual overrides with `override`. Always mark `const` methods `const`. Never omit either.

## Git

NEVER run `git checkout <file>` or `git restore <file>` to revert changes. It destroys uncommitted work. To undo only your own edits, use Edit to manually revert the specific lines you changed.

## Build

Build only when explicitly requested.
Use `.\build.bat` for builds.
Avoid `cmake --build build` for verification.

## Codebase Navigation

Game world/state: `engine/egameboard.*` handles tile/building changes, money, undo, city/player checks, terrain scheduling.

Text strings: `zeus-text strings/Zeus_Text.xml` READ ONLY - reference strings at runtime, re-use for messages.

Options menu hotkeys: Add `eHotkeyId` + setting in `esettings.h/cpp`, handler in `egamewidget.cpp keyPressEvent`, menu entry in `eoptionsdata.cpp getOptionsPages()`.

Popup buttons: Prefer `eAcceptButton`/`eCancelButton` for dialogs; `eOkButton` is smaller/older.

<!-- lean-ctx-compression -->
OUTPUT STYLE: concise
- Bullet points over paragraphs
- Skip filler words and hedging ("I think", "probably", "it seems")
- 1-sentence explanations max, then code/action
- No repeating what the user said
<!-- /lean-ctx-compression -->
