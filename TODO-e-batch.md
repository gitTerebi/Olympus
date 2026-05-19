# E Game Events Save Migration
Source: `TODO-save-migration.md` E1-E12.
Goal: every game event save is tagged, named, bounded, and safe to add/remove/reorder.

## Target Shape

Current:
```
event.N {
  type
  branch
  eventPayload = blob {
    raw event bytes
    raw date bytes
    raw city/building refs
    raw helper value bytes
  }
}
```

Target:
```
event.N {
  type
  branch
  state {
    named base fields
    named child fields
    typed refs
    named helper value fields
  }
}
```

No `eventPayload` blob.
No raw date bytes.
No raw city/building refs.
No raw helper value bytes.
No subclass `read(eReadStream&)` / `write(eWriteStream&)`.
No nested helper `read(eReadStream&)` / `write(eWriteStream&)`.
No `readStream()` / `writeStream()` in migrated event save code.
No second reader shape.

## Migration Order

1. [ ] Add date archive helper if needed.
   - Prefer `eDate::serialize(eSaveArchive& ar)` or `eSaveArchive::dateField()`.
   - Fields are named, e.g. `year`, `month`, `day`.
   - No raw `eDate::read/write` calls from migrated event code.

2. [ ] Convert `eGameEvent` to P1 owned polymorphic root.
   - Base owns `read(eReadStream&) final`.
   - Base owns `write(eWriteStream&) const final`.
   - Add protected virtual `serializeFields(eSaveArchive& ar)`.
   - Base `read/write` opens one `eSaveArchive`.
   - Base calls `serializeFields(ar)`.
   - Children stop overriding `read/write`.

3. [ ] Migrate `eGameEvent` base fields.
   - Rename saved field names away from member prefixes.
   - Use explicit defaults on renamed/addable fields.
   - `ioId` default `-1`.
   - `datePlusDays`, `datePlusMonths`, `datePlusYearsMin`, `datePlusYearsMax` default `0`.
   - `periodDaysMin`, `periodDaysMax` default `100`.
   - `warningMonths` default `2`.
   - `remainingRuns` default `0`.
   - `reason` default empty string.
   - `episodeCompleteEvent` default `false`.
   - `nextDate` uses named date helper.

4. [ ] Rewrite consequences array.
   - Keep `type`.
   - Keep `branch`.
   - Replace `eventPayload` with `state`.
   - Reader creates event from `type` + `branch`.
   - Reader calls event root entry point on `state`.
   - Writer calls event root entry point on `state`.
   - Set parent/reason after child creation as current semantics require.

5. [ ] Convert `eWarning`.
   - If warnings stay nested helpers, remove `read/write`.
   - Add `serialize(eSaveArchive& ar)` or `serializeFields(ar)`.
   - `nextDate` uses named date helper.
   - Rename fields: `finished`, `warningMonths`.
   - Use defaults: `finished=true`, `warningMonths=2`.

6. [ ] Convert event value helpers to helper roots.
   - `eCityEventValue`
   - `eCountEventValue`
   - `eResourceEventValue`
   - `eAttackingCityEventValue`
   - `eMonsterEventValue`
   - `eMonstersEventValue`
   - `ePointEventValue`
   - `eGodEventValue`
   - `eGodReasonEventValue`
   - Remove helper `read/write`.
   - Add `serialize(ar)` / `serializeFields(ar)`.
   - Parent helper fields are saved before child fields.

7. [ ] Replace raw refs.
   - World city refs use typed world-city helper.
   - Board city refs use typed city helper.
   - Building refs use `buildingField` / `buildingAsField`.
   - Tile refs use `tileField`.
   - Resource/god/monster enums use named `field()`.

8. [ ] Convert `eEventTrigger`.
   - Remove `payloadField` from trigger state.
   - Use named fields and typed refs.
   - Keep runtime parent/board out of save state.

9. [ ] Subclass batch 1: army/raid/conquest.
   - `earmyeventbase`
   - `earmyreturnevent`
   - `eattackingcityeventvalue`
   - `eplayerconquesteventbase`
   - `eplayerconquestevent`
   - `eplayerraidevent`
   - `erivalarmyawayevent`

10. [ ] Subclass batch 2: disaster/landscape.
    - `eearthquakeevent`
    - `elandslideevent`
    - `elavaevent`
    - `esinklandevent`
    - `etidalwaveevent`

11. [ ] Subclass batch 3: economic/trade/wage.
    - `edemandchangeevent`
    - `eeconomicchangeevent`
    - `eeconomicmilitarychangeeventbase`
    - `epricechangeevent`
    - `esupplychangeevent`
    - `esupplydemandchangeevent`
    - `ewagechangeevent`
    - `etradeopenupevent`
    - `etradeshutdownevent`
    - `emilitarychangeevent`

12. [ ] Subclass batch 4: city/resource interactions.
    - `ecitybecomesevent`
    - `egiftfromevent`
    - `egifttoevent`
    - `ereceivetributeevent`
    - `emakerequestevent`
    - `eraidresourceevent`
    - `eresourcegrantedeventbase`

13. [ ] Subclass batch 5: requests/aid/reinforcements.
    - `erequestaidevent`
    - `erequeststrikeevent`
    - `ereinforcementsevent`
    - `etroopsrequestevent`
    - `etroopsrequestfulfilledevent`
    - `requests/e-fulfill-request-event`
    - `requests/e-pay-tribute-event`

14. [ ] Gods family.
    - `gods/egodattackevent`
    - `gods/egoddisasterevent`
    - `gods/egodquest`
    - `gods/egodquesteventbase`
    - `gods/egodquestevent`
    - `gods/egodquestfulfilledevent`
    - `gods/egodtraderesumesevent`
    - `gods/egodvisitevent`
    - `gods/egodeventvalue`
    - `gods/egodreasoneventvalue`

15. [ ] Invasions family.
    - `invasions/invasion-event`
    - `invasions/invasion-handler`
    - `invasions/invasion-warning`
    - `invasions/monster-in-city-event`
    - `invasions/monster-invasion-event-base`
    - `invasions/monster-invasion-event`
    - `invasions/monster-unleashed-event`

16. [ ] Final cleanup.
    - Remove orphan `read/write` declarations/defs.
    - Remove event payload save path.
    - Remove raw helper calls.
    - Update `TODO-save-migration.md` E status only after scans pass.

## Type Groups

- [ ] Base event + consequences.
- [ ] Warning helpers.
- [ ] Trigger helpers.
- [ ] Value helpers.
- [ ] Army/raid/conquest events.
- [ ] Disaster/landscape events.
- [ ] Economic/trade/wage events.
- [ ] City/resource interaction events.
- [ ] Requests/aid/reinforcement events.
- [ ] Gods events.
- [ ] Invasion/monster events.

## Per-Type Checklist

- [ ] Fields are named.
- [ ] Field names are stable and durable.
- [ ] New field names do not use member prefixes like `m`.
- [ ] Renamed/addable fields use explicit defaults.
- [ ] Parent fields serialize before child fields.
- [ ] Runtime parent/board/cache fields are not saved.
- [ ] Direct refs use typed helpers.
- [ ] Collections use `arrayField` / `fixedArrayField` / `countedArrayField`.
- [ ] No duplicate refs remain.
- [ ] No raw event payload remains.
- [ ] No raw date bytes remain.
- [ ] No raw helper bytes remain.
- [ ] No subclass `read/write` remains.
- [ ] Smoke save/load when feasible.

## E-batch Status

- Initial audit only.
- Known raw roots: `eGameEvent`, `eWarning`, `eEventTrigger`, event value helpers, many subclasses.
- Build passed after F-tile work, but E not migrated.
- Smoke save/load not run.

## Stop Conditions

- Payload ownership is unclear.
- Consequence child creation needs a field before `type` + `branch`.
- A value helper is shared outside events and cannot be safely reshaped.
- A city/ref helper does not exist and field shape is unclear.
- Runtime callback/action state appears to carry durable gameplay meaning.
- Field name is unclear.
- Post-load invariant is unclear.

## Verification

```powershell
rg -n "eventPayload|payloadField\(" gameEvents
rg -n "readStream\(|writeStream\(|legacyReadStream\(|\.val\(" gameEvents
rg -n "void\s+(read|write)\s*\(\s*e(Read|Write)Stream" gameEvents
rg -n "field\(\"m" gameEvents
git diff --check
```

Build only when asked:
```powershell
.\build.bat
```
