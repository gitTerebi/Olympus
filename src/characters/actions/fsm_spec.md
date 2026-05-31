# Cart FSM Spec

## Design principles
- State transition logic lives in `decide()` only — pure inputs → next state
- Actions live in `enter_X()` only — side effects, no branching
- 1 state = 1 action

---

## DELIVER FSM (eDeliverCartAction)

| State | Action (enter_X) | Next state | Condition |
|---|---|---|---|
| `idle` | (none) | `loading` | always |
| `loading` | top-up stock to capacity from building | `waitOutside` | has resource after take |
| `loading` | top-up stock to capacity from building | `idle` | no resource + nothing to take |
| `waitOutside` | walk to adjacent road tile; no road → adjacent tile | `findTarget` | arrived |
| `findTarget` | async BFS pathfind to target with space | `moving` | path found |
| `findTarget` | sleep(kRetryWaitTicks) | `findTarget` | fail, retry < N |
| `findTarget` | (none) | `returning` | fail, retry = N |
| `moving` | walk to target building | `atTarget` | arrived |
| `atTarget` | deliver resource to target | `returning` | all delivered (resCount == 0) |
| `atTarget` | deliver resource to target | `idleOutside` | partial (target full, resCount > 0) |
| `idleOutside` | sleep(kRetryWaitTicks) | `findTarget` | always (after sleep) |
| `returning` | walk home | `loading` | has stock on arrive |
| `returning` | walk home | `idle` | no stock on arrive |

---

## GET FSM (eGetCartAction)

| State | Action (enter_X) | Next state | Condition |
|---|---|---|---|
| `idle` | (none) | `findTarget` | always |
| `findTarget` | async BFS pathfind to target with stock | `moving` | path found |
| `findTarget` | sleep(kRetryWaitTicks) | `findTarget` | fail (retry forever) |
| `moving` | walk to target building | `atTarget` | arrived |
| `atTarget` | take max from target | `returning` | always (0 taken = empty race, still return) |
| `returning` | walk home + deposit to building + stash overflow | `idle` | always |

---

## Constants
- `kRetryWaitTicks = 1000`
- `kMaxDropoffRetries = 250` (DELIVER findTarget only)

---

## Notes
- DELIVER cart never deposits stock back to home building — keeps retrying delivery
- GET cart always deposits on return, stash overflow if building full
- `waitOutside` falls back to any adjacent tile if no road exists
- `findTarget` BFS only returns targets with space (deliver) or stock (get) — no same-target loops
- State transition in `decide()`, action in `enter_X()`, never mixed
