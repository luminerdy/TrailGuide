# V3 progress and verification checkpoint

## Current state

**The development device is running `v3-0.1.27-accel16` DRAFT.** Full application readback and exact-version live runtime checks passed. Physical feedback shows a substantial improvement, but **gesture acceptance is partial: Coaching compass shaking still fails, and automatic shaking sometimes fails after path selection; S followed by shaking works in that situation.**

Reported working contexts are Classic compass, Classic card / New, and Coaching card view. These are qualitative observations, not measured success rates or proof of exactly one draw per gesture. The later path-selection report qualifies the Coaching card success: it is not reliable in every reported situation. Flip behavior and ordinary-walking/handling negatives have not been newly established for27.

This is a documentation checkpoint, not a production release or a claim that all gesture failures are solved. **The committed firmware source baseline remains `v3-0.1.22-reliability`.** Later installed work was built in separate, retained source snapshots. This checkpoint does not integrate those snapshots or publish firmware binaries, private build paths, device identifiers, raw captures or deployment credentials.

[Project specification](PROJECT-SPEC.md) · [Delivery plan](PLAN.md) · [Verification record](BRINGUP.md) · [Classic flow](CLASSIC-FLOW.md) · [Coaching flow](COACHING-FLOW.md)

## Progress since the published baseline

- **22 — reliability delivery:** installed, but repeated physical shaking did not meet the intended reliability target. Passing software checks did not close that failure.
- **23 — content-only delivery:** seven scoped wording changes were accepted in the private trial. Motion behavior was unchanged; broader content work remains separate.
- **25 — optional manual S control:** added a plain, unboxed S with a real 48×48 touch target while preserving its subdued appearance. Neither automatic nor S-assisted shaking displayed a card in the physical report.
- **26 — manual acquisition correction:** corrected host-reproduced moving-start and smooth-phase failures in the explicit S-assisted path, preserving the eight-second intent window. Physical shaking still failed despite visible Shake ready feedback. A subsequently captured physical trace established accelerometer clipping as a sufficient cancellation mechanism for the last known armed attempt; it did not establish the cause of every reported failure.
- **27 — accelerometer range correction:** changes the supported accelerometer range from ±4g to ±16g, with matching conversion and range-dependent validity bounds. Only three sensor/recognizer headers changed, apart from the build-version literal. Gesture timing, threshold policy, UI appearance, gyro configuration and unrelated drivers remain unchanged. Physical feedback now reports working shakes in multiple contexts, with the Coaching gaps above still open.

## Captured26 failure and causal limits

A complete retained26 recording contains 4,544 rows / 181,760 bytes with validated whole-buffer CRC. It contains 352 clipped-invalid rows, no recorded gesture triggers and no new read-error delta. The raw rail values agree with the recorded clipping flags.

Replaying the unchanged26 implementation with the exact recorded inputs shows the last known manual arming canceled at a later clipped sample. The chain is raw saturation → unhealthy publication → gesture reset → manual arming cleared. Independent review confirmed that sufficient mechanism while retaining the timing and coverage limitations. Earlier arming times were not invented, and this does not explain every disappearance or every attempt.

The older recording remains labelled ±4g. Saturated peaks cannot be recovered or relabelled as a ±16g trial. One gyro sample also clipped; the gyro range is unchanged. There is **no new27 movement recording**, so the remaining Coaching failures are human observations, not yet trace-attributed causes.

## Intended S-assisted interaction

With a card and its controls showing, tap **S**. The current card should remain visible and **Shake ready** should appear. After release, one qualified deliberate shake should draw exactly one card within the eight-second intent window. Starting while already moving should not require an extra motionless setup sample.

S-assisted success is now reported in the path-selection failure situation. It is a useful fallback and a diagnostic distinction, **not acceptance of automatic shaking everywhere**. Touch remains available. Classic remains the startup default; Classic, the canonical catalog and Coaching remain distinct. The accepted six-second reveal and five-second reading hold are retained. Sound remains optional, RAM-only and OFF on boot.

## Verification completed for27

These are retained implementation and installation results, not firmware tests rerun by this documentation update:

- Source scope: frozen 29-file candidate, three range-related header changes and independent source review. Focused conversion, rail, validity and compatibility probes passed; the original failed cases and causal limits remain retained.
- Full native configuration: 147 child commands passed, with 27 fresh C++ commands, 424 source/configuration/hash-verified reused HOST C objects, 547 exact frame comparisons and 11 protocol tests / zero skips. The task-local26 baseline oracle was explicitly reused, not described as a second fresh baseline run. Raw fixture representations and full-scale safety negatives were reviewed separately from recognizer policy.
- Clean ESP32-S3 target build: 487 actual compiler/build commands passed. The real application and matching archived ELF/map, source bindings and closed package evidence were independently verified. No target SDK object reuse is claimed.
- Installer: 24 guarded offline tests passed in the parent's complete run, followed by an independent exact-helper review with additional adversarial checks. A shorter interrupted parent test invocation remains recorded as incomplete, not relabelled a pass. Current26, candidate27 and retained recovery26 roles are explicit and archive-aware.
- Preservation: before serial access or reset, fresh named26 recorder headers matched the existing complete export. Its full bytes/CRC were validated and reused; the valuable recording was not discarded or re-recorded.
- Actual installation: only the application at `0x10000` was written. The full 695,712-byte readback matched before the native USB boot reset. Compatible bootloader and partitions were not rewritten. The exact previous26 installed record and artifacts remain retained; no automatic rollback was performed.
- Actual runtime: exact27 ROM identity, initialized active Classic UI, advancing application/compass/sensor counters and valid physically plausible acceleration passed two-snapshot checks. Boot mute and empty/off recorder defaults passed; execution was resumed. Running confirms the checked configuration initialization path, not a separate direct register probe or successful physical gesture.

Application SHA-256:

```text
17802c5e8eeabfd54fd9477565aeb3c47e206803210180eaf50c7f622e2e72cd
```

No fresh isolated 128-test integration run is claimed for26 or27; the older run belongs to its original release scope. Build, transfer and runtime evidence do not establish exhaustive visible rendering, reliable repeated gestures, exactly-once physical behavior, battery endurance or sustained field reliability.

## Verified off-device follow-up

A focused actual-LVGL comparison against the installed27 build-bound source now demonstrates a deterministic post-navigation no-stillness requirement gap: all six real pointer-selected Coaching paths reject an immediate automatic burst without renewed quiet acquisition, while the identical burst succeeds after renewed acquisition or a real released S tap. Coaching compass and Classic compass/card positive controls also pass after acquisition; no unconditional Coaching-compass dispatch block was found in these tested routes.

The parent re-executed the source/fixture/binary-bound 27-row mechanism/control loop successfully and the separately labelled unchanged-baseline contract loop as intentional RED. This is not reproduction of the unrecorded27 physical incident and does not explain every reported context difference. Quiet positives are diagnostic controls, not a new stillness requirement for the user. The original link-budget overrun remains disclosed; independently repeated replay-only loops stayed within their envelope.

See [the focused findings and scoped correction proposal](V3-GESTURE-FOLLOWUP.md). No policy candidate or firmware change has been implemented. Bounded independent-scroll/inertia and acquisition-versus-rearm checks are the next off-device coverage step; actual incident attribution remains open.

## Next steps — off-device first

1. **Preserve the working improvement.** Keep27 installed and leave the device untouched while physical testing is paused. No reset, reflash, new recording, recovery installation or unrelated tuning follows from this checkpoint.
2. **Compare contexts in the exact installed source.** Trace Classic compass, Coaching compass and Coaching card paths through sample health, pointer/scroll eligibility, detector reference/acquisition/rearm and Start Fresh dispatch. Compare automatic recognition after path selection with the S-assisted path; do not assume their different outcomes identify a cause.
3. **Build a focused, red-capable comparison.** Exercise identical labelled synthetic inputs across those contexts with working positive controls and pointer/scroll/fault negatives. Separate a deterministic requirement counterexample from reproduction of the unrecorded27 physical incident. Keep the older ±4g capture immutable.
4. **Propose the smallest evidenced correction.** Retain the ±16g gain, UI and global Start Fresh contract. Do not lower thresholds blindly, add a stillness ritual or require S as the normal route. Present the diagnosis, scoped change and validation plan before implementation or another installation.
5. **Check a future approved candidate in layers.** Require focused composed regressions, independent review and applicable full native/target/package checks; installation consent, readback/runtime and physical acceptance remain separate gates.
6. **Minimize the next human check.** When testing resumes, prepare a separately approved short recording focused on Coaching compass, then the path-entry distinction if still needed. Do not request more movements during the current pause. Expand repeatability and ordinary-handling checks only after the remaining positive paths are understood.
7. **Publish later source separately.** This progress update is not a source backup or integration of the private firmware or unrelated editorial work. Any later source publication needs its own checked, privacy-reviewed scope.

The primary project remains learner-owned, project-led mentoring through **Define → Direct → Check → Adjust**. The device is an optional playful companion, not a curriculum prerequisite, diagnostic instrument or evidence of learning outcomes.
