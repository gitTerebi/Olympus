# AGENTS.md

## Code Style

Refactor repeated functions into shared local helpers where possible.
Debug log using printf, not std out.
New files: kebab-case, no `e` or `e-` prefix (e.g. `storage-widget.cpp` not `estorage-widget.cpp` or `e-storage-widget.cpp`).

## JSON Save Transition

Prefer JSON props over `blob` for small, self-contained state that already has `serializeJson(eJsonArchive&)`.
Do not convert blob state to JSON unless every nested type/ref it saves has JSON support.
Good small candidates: flat structs/classes with existing `serializeJson`, no post-load refs, no nested action graphs.
Bad candidates until expanded: characters/actions, players with event refs, tiles, and large systems with many raw `read/write` deps.
When storing counted JSON objects with keys like `x.count`, store children under object paths (`x.0`, `x.1`) using `ar.child("x.N")`.
Do not use `ar.childAt("x", i)` when the same key also stores `x.count`; `childAt` expects an array and will conflict with object-style count fields.
Use `childAt` only with a separate array key, e.g. `thingCount` + `things[]`.
Keep read/write JSON shapes identical. If read uses `ar.child("x.N")`, write must use the same.
For old/new JSON format transitions, first decide if old saves must load; if yes, add explicit compat based on field presence, not a blind blob fallback.
After save-format edits, test loading a save made before the edit and a save made after the edit.

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
