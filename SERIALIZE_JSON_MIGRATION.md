# serializeJson Migration — Characters & Actions

## Goal
Replace binary `read(eReadStream&)` / `write(eWriteStream&)` / `serialize(eSaveArchive&)` with
`serializeJson(eJsonArchive& ar)` on all character and action classes.
Remove blob-based character serialization from the board's JSON path.

## Why
- `serialize(eSaveArchive&)` produces an opaque binary blob stored inside JSON — defeats the point
- `serializeJson` writes human-readable JSON fields directly

---

## Rules

### 1. Merge read + write into one serializeJson
```cpp
// BEFORE
void Foo::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}
void Foo::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<Foo*>(this)->serialize(ar);
}
void Foo::serialize(eSaveArchive& ar) {
    ar.field("mX", mX);
    ar.field("mY", mY);
}

// AFTER
void Foo::serializeJson(eJsonArchive& ar) {
    ar.field("mX", mX);
    ar.field("mY", mY);
}
```

### 2. Always call base class serializeJson first
```cpp
void Derived::serializeJson(eJsonArchive& ar) {
    Base::serializeJson(ar);   // <-- first
    ar.field("mDerivedField", mDerivedField);
}
```

### 3. Replace src.readXxx / dst.writeXxx with ar equivalents

| Old (binary) | New (JSON) |
|---|---|
| `src.readCharacter(&board(), cb)` | `ar.addPostFunc([&]{ cb(lookup by ioID) })` — see note |
| `dst.writeCharacter(mPtr)` | `ar.field("charIOID", mPtr ? mPtr->ioID() : -1)` |
| `src.readBuilding(&board(), cb)` | `ar.buildingRef("key", cb, board())` |
| `dst.writeBuilding(mPtr)` | `ar.buildingRef("key", mPtr, board())` |
| `ar.tile(mTile, board())` *(eSaveArchive)* | `ar.tile("mTile", mTile, board())` *(eJsonArchive)* |
| `ar.characterAction<T>(mA, factory)` | see note below |

**Character cross-refs:** `eJsonArchive` has no `readCharacter`/`writeCharacter`.
Pattern to use:
```cpp
// write side (ar.writing())
int id = mCharPtr ? mCharPtr->ioID() : -1;
ar.field("charIOID", id);

// read side (ar.reading()) — deferred because char may not exist yet
ar.addPostFunc([this, &board = board()]() {
    int id = ...; // stored during read pass
    board.postCharacter(id, [this](eCharacter* c){ mCharPtr = c; });
});
```
Or store ioID in a local then resolve in postFunc.

**characterAction (mAction, mFinishAction, etc.):**
These are `eCharActFunc` subtypes serialized by type enum + fields.
Pattern:
```cpp
// writing
int type = mAction ? static_cast<int>(mAction->type()) : -1;
ar.field("actionType", type);
if(mAction) mAction->serializeJson(ar.child("action"));

// reading
int type = -1;
ar.field("actionType", type);
if(type >= 0) {
    mAction = eCharacterAction::sCreate(this, static_cast<eCharActionType>(type));
    mAction->serializeJson(ar.child("action"));
}
```

### 4. Conditional read/write blocks → use ar.reading() / ar.writing()
```cpp
// BEFORE
if(ar.reading()) {
    int n; ar.field("n", n);
    for(int i = 0; i < n; i++) { ... push_back ... }
} else {
    int n = vec.size(); ar.field("n", n);
    for(auto& x : vec) { ... }
}

// AFTER — same pattern, ar.reading()/ar.writing() work on eJsonArchive too
if(ar.reading()) { ... }
else { ... }
```

### 5. Header changes
```cpp
// Add to .h (public):
void serializeJson(eJsonArchive& ar);   // or override if base declares virtual

// Add forward decl if needed:
class eJsonArchive;

// Remove:
void read(eReadStream& src);
void write(eWriteStream& dst) const;
// Remove private:
void serialize(eSaveArchive& ar);
```

### 6. Include changes in .cpp
```cpp
// Add:
#include "fileIO/ejsonarchive.h"
// Remove (if no longer used):
#include "fileIO/esavearchive.h"
```

---

## Board JSON path (e-game-board-read.cpp)

### Read side — replace blob (lines ~585-596):
```cpp
// BEFORE
std::string blob;
ar.field(("mCharacters." + std::to_string(i) + ".blob").c_str(), blob);
const auto c = eCharacter::sCreate(type, *this);
replayRead(blob, [&c](eReadStream& s){ c->read(s); });

// AFTER
const auto c = eCharacter::sCreate(type, *this);
auto cAr = ar.childAt("mCharacters", i);
cAr.field("type", type);  // already read above, skip or move inside
c->serializeJson(cAr);
```

### Write side — replace blob (lines ~839-850):
```cpp
// BEFORE
ar.field(("mCharacters." + std::to_string(i) + ".type").c_str(), type);
std::string blob = captureWrite([&](eWriteStream& d){ c->write(d); });
ar.field(("mCharacters." + std::to_string(i) + ".blob").c_str(), blob);

// AFTER
auto cAr = ar.childAt("mCharacters", i);
cAr.field("type", type);
c->serializeJson(cAr);
```

---

## Order of migration (bottom-up hierarchy)

1. `eCharacterBase` (echaracterbase.h / .cpp)
2. `eCharacter` (echaracter.h / .cpp)
3. `eCharacterAction` (actions/echaracteraction.h / .cpp)
4. Action leaf classes (one per file, ~48 files)
5. Character subclasses (ecattle, etrailer, etc.)
6. Board JSON path (e-game-board-read.cpp)

---

## Do NOT touch
- Binary board read/write path (the `eReadStream`/`eWriteStream` board functions)
- `eCharacterBase::read` / `write` can be removed once all callers gone
- Missile serialization (separate task)
