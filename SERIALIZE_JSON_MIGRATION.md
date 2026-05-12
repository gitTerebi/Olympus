# serializeJson Migration — Characters & Actions

## Goal
Replace binary `read(eReadStream&)` / `write(eWriteStream&)` / `serialize(eSaveArchive&)` with
`serializeJson(eJsonArchive& ar)` on all character and action classes.
Remove blob-based character serialization from the board's JSON path.

## Why
- `serialize(eSaveArchive&)` produces an opaque binary blob stored inside JSON — defeats the point
- `serializeJson` writes human-readable JSON fields directly

tilesBlob — whole tile grid (tiles have no serializeJson yet, keep as blob for now)
mEarthquakes — captureWrite blob → has tile refs, needs ar.tile()
mTidalWaves — captureWrite blob → has tile refs
eLavaFlow — captureWrite blob → has tile refs
eLandSlide — captureWrite blob → has tile refs
