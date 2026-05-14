# Popup Event Responses TODO

## Handoff Context

- Crash seen in `eGameBoard::removeGameEvent(eGameEvent*)`.
- Repro: invasion popup -> click bribe/fight/surrender -> load game -> sometimes crash.
- Root cause: popup/button/close lambdas copy `eEventData`; `eEventData` owns lambdas; lambdas capture event/self; event can outlive board and later destroys through stale board ref.
- `markBoardDead` guard was tried and rejected. Do not reintroduce it.
- Desired fix is source-level ownership cleanup, not destructor guard.
- Current dirty tree may contain interim board action-registry changes from this session.
- New session should either finish converting that interim design into event-owned responses, or cleanly revert interim registry before implementing final design.
- Save helper changes are separate work: `eSaveArchive::gameEvent()` and pointer helpers were added/adjusted to reduce raw stream writes.
- Build rule from `AGENTS.md`: build only when explicitly requested, use `.\build.bat`.
- `zeus-text strings/Zeus_Text.xml` is read-only.

## Goal

- Popup UI must not own gameplay lambdas.
- Popup UI must not keep `stdsptr<eGameEvent>` alive.
- Popup UI should carry only display data, runtime event/action id, response id, and optional city id.
- Live domain object owns response logic.
- `eGameBoard` routes popup response to the live object and ignores stale ids.

## Core API

- Add runtime ids to `eGameEvent`, separate from save `ioID`.
  - `int runtimeId() const`.
  - Assigned in `eGameBoard::addGameEvent`.
  - Reset/invalidated only by event destruction/board clear.
- Add board lookup:
  - `eGameEvent* eventWithRuntimeId(int id) const`.
- Add response API:
  - `virtual void eGameEvent::respond(int response, eCityId city = eCityId::neutralAggresive);`
  - Default no-op.
  - `void eGameBoard::respondToEvent(int runtimeId, int response, eCityId city = eCityId::neutralAggresive);`
- Add popup response fields to `eEventData`:
  - `int fEventRuntimeId = -1;`
  - `int fPrimaryResponse = -1;`
  - `int fSecondaryResponse = -1;`
  - `int fTertiaryResponse = -1;`
  - `std::map<eCityId, int> fCityConditionalResponses;`
  - Optional: `int fCloseResponse = -1;`

## Popup UI Changes

- `e-message-box.cpp` buttons call `board.respondToEvent(ed.fEventRuntimeId, response, city)`.
- Popup close/default behavior also uses response ids:
  - invasion default close -> fight.
  - request/tribute/troops default close -> existing current fallback behavior.
- Message log/toast/history must clear runtime response ids.
- Stored history is display-only.

## Event Conversions

- `eInvasionEvent`
  - Add enum: surrender, bribe, fight.
  - Move existing lambdas into methods:
    - `surrender()`
    - `bribe()`
    - `fight()`
  - `trigger()` fills `fEventRuntimeId` + response ids.
  - No lambda captures `self`.

- `ePayTributeEvent`
  - Add enum: dispatch, postpone, refuse.
  - City-specific dispatch uses `respond(dispatch, cid)`.
  - Existing `dispatch`, `postpone`, `finish(refuse)` become response targets.

- `eReceiveTributeEvent`
  - Add enum: accept, postpone, decline.
  - Move logic now in `eTributeHelpers::receiveTributeFromCity` lambdas into event methods.
  - `trigger()` should build popup directly or call helper that fills response ids.

- `eFulfillRequestEvent`
  - Add enum: dispatch, postpone, refuse.
  - City-specific dispatch uses `respond(dispatch, cid)`.
  - Existing `dispatch`, `postpone`, `finish(refuse)` become response targets.

- Resource granted / request granted flow
  - If it is owned by `eResourceGrantedEventBase`, add `respond()` there.
  - Otherwise make it an event-owned or board-owned popup action before removing lambdas.

- `eTroopsRequestEvent`
  - Add enum: dispatchNow, postpone, refuse.
  - `dispatch(close)` is special because UI close callback is passed in.
  - Replace with event method that performs dispatch, then popup closes after response.
  - If dispatch must close later, use a response result enum from `respond()`.

## Tribute Naming

- Pay tribute:
  - `gameEvents/requests/e-pay-tribute-event.*`
  - `eGameEventType::payTribute`
  - class `ePayTributeEvent`
- Receive tribute:
  - `gameEvents/ereceivetributeevent.*`
  - `eGameEventType::receiveTribute`
  - class `eReceiveTributeEvent`
- Helper:
  - `engine/e-tribute.*`
  - `eTributeHelpers::payTribute`
  - `eTributeHelpers::receiveTribute`
  - `eTributeHelpers::receiveTributeFromCity`
  - `eTributeHelpers::payTributeToCity`

## Remove Interim Registry

- Remove these if present:
  - `prepareMessageActions`
  - `runMessageAction`
  - `runMessageCloseAction`
  - `clearMessageActions`
  - `mMessageActions`
  - `mMessageCloseActions`
  - `fPrimaryActionId`, `fSecondaryActionId`, etc. if replaced by response ids.
- Remove action lambdas from `eEventData` once all senders are converted:
  - `fPrimaryAction`
  - `fSecondaryAction`
  - `fTertiaryAction`
  - `fCloseOnAction`
  - `fCityConditionalActions`

## Verification

- Search checks:
  - no popup code calls `ed.f*Action`.
  - no popup captures `ed` containing lambdas.
  - no event popup sets `fPrimaryAction`, `fSecondaryAction`, `fTertiaryAction`, `fCloseOnAction`, or `fCityConditionalActions`.
  - no `stdsptr<eGameEvent>` captured by popup lambdas.
- Manual test:
  - invasion popup -> bribe -> load game.
  - invasion popup -> fight -> load game.
  - invasion popup -> surrender -> load game.
  - pay tribute popup all buttons.
  - receive tribute popup all buttons.
  - fulfill request popup all buttons.
  - troops request popup all buttons.
- Build only when explicitly requested.
