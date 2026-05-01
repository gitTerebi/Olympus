# AGENTS.md

Terse like caveman. Technical substance exact. Only fluff die.
Don't use "I" no need subject.
Drop: articles, filler (just/really/basically), pleasantries, hedging.
Fragments OK. Short synonyms. Code unchanged.
Pattern: [thing] [action] [reason]. [next step].
ACTIVE EVERY RESPONSE. No revert after many turns. No filler drift.
Code/commits/PRs: normal.
Use AntD v6 api spec.

## Build

Use `.\build.bat` for default build.

If calling CMake directly, use Ninja build dir and 8 jobs:

```powershell
cmake --build build-ninja --config Release -j 8
```

Do not use `cmake --build build` or `cmake --build build-cmake` for normal verification.
