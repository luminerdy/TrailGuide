# V3 progress and verification checkpoint

## Current state

**The development device is running `v3-0.1.26-manual-s` DRAFT.** Its complete application readback and exact-version live runtime checks passed. **Physical S-assisted shake acceptance is pending. Automatic shake reliability remains open.**

This is a documentation checkpoint, not a production release or a claim that the reported gesture failure is solved. The committed firmware baseline remains `v3-0.1.22-reliability`; later installed work was built in separate, retained source snapshots. This checkpoint does not integrate those snapshots or publish firmware binaries, private build paths, device identifiers, raw captures or deployment credentials.

[Project specification](PROJECT-SPEC.md) · [Delivery plan](PLAN.md) · [Verification record](BRINGUP.md) · [Classic flow](CLASSIC-FLOW.md) · [Coaching flow](COACHING-FLOW.md)

## Progress since the published baseline

- **22 — reliability delivery:** installed, but repeated physical shaking did not meet the intended reliability target. Passing software checks did not close that failure.
- **23 — content-only delivery:** seven scoped wording changes were accepted in the private trial. Motion behavior was unchanged; broader content work remains separate.
- **25 — optional manual S control:** added a plain, unboxed S with a real 48×48 touch target while preserving its subdued appearance. The physical report was that neither automatic nor S-assisted shaking displayed a card. That failure remains part of the evidence.
- **26 — manual acquisition correction:** changes only the explicit S-assisted acquisition path. It addresses host-reproduced moving-start and smooth-phase failures without requiring a stationary first sample. Automatic recognition, sensor drivers, UI appearance, thresholds and duration-credit policy are unchanged from the preceding manual-S candidate.

The original human incident was not captured in a usable movement trace. Synthetic requirement failures were reproduced and corrected, but that does not establish the cause of the human failure. Historical S request delivery is also unresolved; present eligibility alone cannot reconstruct the earlier tap.

## Intended S-assisted interaction

With a card and its controls showing, tap **S**. The current card should remain visible and **Shake ready** should appear. After release, one qualified deliberate shake should draw exactly one card within the eight-second intent window. Starting while already moving should not require an extra motionless setup sample.

This is the interaction contract, **not a newly confirmed physical result**. Touch remains an alternative. Classic remains the startup default; Classic, the canonical catalog and Coaching remain distinct. The accepted six-second reveal and five-second reading hold are retained. Sound remains optional, RAM-only and OFF on boot.

## Verification completed for 26

These are retained implementation and installation results, not tests rerun by this documentation update:

- Source/host review: 974 strict checks and 20 independent boundary checks passed. Separate square-wave and sine-wave sweeps each covered 18,720 scenarios with no misses in the tested matrix. These are synthetic scenarios, not physical trials.
- Focused actual-LVGL integration: 54 cases / 639 checks passed, including established pointer-contact cancellation.
- Full native configuration: 147 child commands completed successfully, including 27 freshly compiled C++ commands, 424 source/configuration/hash-verified reused host C objects, 547 generated frame comparisons and 11 protocol tests with zero skips. The reused host objects were not target SDK objects.
- Clean ESP32-S3 target build: 487 compiler/build commands completed. The actual application and matching archived ELF/map were independently package-reviewed; retained archive members were hash-verified.
- Installer: 22 guarded offline tests passed, followed by an independent hash-bound review. A hexadecimal-output NotReady retry defect was reproduced, corrected and re-reviewed without changing the firmware payload. Strict runtime checks were not weakened.
- Actual installation: only the application at `0x10000` was written. The full 695,712-byte readback matched before the native USB boot reset. Compatible bootloader and partitions were not rewritten.
- Actual runtime: exact version26 identity, initialized active Classic UI, progressing compass/application/sensor counters and valid sensor samples passed two-snapshot checks. Boot mute and empty/off recorder defaults passed; execution was resumed. Previous25 evidence was retained. No new motion recording or automatic rollback was performed.

Application SHA-256:

```text
7955bb4643674e5420709f7f0854eb904fa7cead98608aeb6344cca983407a27
```

No fresh isolated 128-test integration run is claimed for26; the older run belongs to its original release scope. Build, transfer and runtime evidence do not establish visible rendering, physical touch/shake success, battery endurance or sustained field reliability.

## Open acceptance and next steps

1. First establish whether tapping S produces **Shake ready** while retaining the card. Then assess the deliberate burst and one-card result; record the actual observation rather than infer it from the host tests.
2. Evaluate moving starts, repeated separate bursts and ordinary-handling negatives as distinct checks when practical. Automatic shake and the earlier intermittent turnover issue remain open; this update does not claim either is repaired.
3. Keep the installed setup stable while physical testing is deferred. Further recording, reset, recovery installation or unrelated tuning requires its own scope; this publication performs none of those operations.
4. Integrate and publish later source only as a separately checked, privacy-reviewed change. Content publication and deferred delivery work are not silently included in this checkpoint.

The primary project remains learner-owned, project-led mentoring through **Define → Direct → Check → Adjust**. The device is an optional playful companion, not a curriculum prerequisite, diagnostic instrument or evidence of learning outcomes.
