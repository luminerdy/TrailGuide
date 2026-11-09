# V3 progress and verification checkpoint

## Current state

**`v3-0.1.28-controls` DRAFT is actually installed.** Both modes now use a **2000ms settled card-only reading hold** after the unchanged **6000ms whole-face reveal**. Only the two UI-header hold values and release identity changed from installed27; the other27 of29 source files, content, gestures, sensors, appearance and sound are unchanged. Touch early completion and navigation are retained.

No new28 physical timing, gesture or readability acceptance exists. Inherited27 reports remain partial: Classic compass/card and Coaching card shaking worked; Coaching compass shaking failed, and automatic shaking was intermittent after path selection while S then shake worked. The Coaching incident remains **UNSOLVED**, with no27/28 field movement trace.

The committed firmware source remains `v3-0.1.22-reliability`. Installed28 and archived recovery27 are separate private source/artifact snapshots, not integrated by this documentation checkpoint. The exact previous27 installed record is retained; no automatic rollback or recovery write occurred.

[Project specification](PROJECT-SPEC.md) · [Delivery plan](PLAN.md) · [Verification record](BRINGUP.md) · [Classic flow](CLASSIC-FLOW.md) · [Coaching flow](COACHING-FLOW.md)

## Verification completed for28

These are retained actual implementation/install results, not firmware checks rerun for this documentation edit:

- Focused actual-LVGL timing checks: **442 assertions passed**, including1999/2000ms boundaries. The original old-hold fixture is retained RED; the27 baseline retained its5000ms pass. This was not initially all green.
- Full canonical native run: **147 actual commands**, **27 fresh sanitized C++ builds**, **424 verified HOST C objects**, **547 exact unchanged-oracle frame comparisons**, and **11 protocol cases / zero skips**. The first unadapted5000ms fixture failure is preserved; the owned fixture changes retain the required range encoding and exact version checks while updating only the affected reading-hold expectations. No fresh baseline run is claimed.
- Clean S3 build: **487 actual compiler/build commands**; independently read-back closed package with **727 archive members /61 sealed files**, matching application and archived ELF/map/source bindings.
- Installer review initially **failed** on an obsolete test expecting the finalized genuine28 map to be absent. The failed review and original test remain preserved. One test-only correction strengthened actual-map binding; the independent unblock review then passed **25/25**, plus genuine authorization composition stopped immediately before enumeration. Those were off-device checks, separate from the later actual installation.
- Actual application-only write at `0x10000`: the complete **695,728-byte readback** matched SHA-256 before native USB boot reset. Compatible bootloader and partitions were not rewritten. Parent independent full-readback hashing also matched.
- Actual runtime: exact28 ROM identity and matching archived ELF/map, initialized active Classic UI, advancing application/compass/IMU counters and valid IMU samples passed strict two-snapshot checks on the first probe. Boot mute and empty/off recorder passed; execution was resumed. These establish liveness, not physical gesture or screen acceptance.

Application/readback SHA-256:

```text
c25be6578548b6d0f9e57ea74b4c9fb4385dced139d0f83ddf9c8d96f8c4a0ad
```

## Observer attempts and remaining work

The27 observer's **53 host cases +11 bridge cases** remain scoped off-device results, not live readiness. Two actual27 starts stopped **before Record/Go**. The second incorrectly reported non-Classic context despite human-confirmed Classic: the failed gate was IMU validity, not evidence of the wrong screen. A cleanup resume-only/not-halted alarm was also misleading; independent full read/resume confirmed healthy valid idle operation. No27/28 movement recording resulted.

Keep installed28 stable. Before any future authorized capture, rebind and review the observer against28's exact application/ELF/map; preserve the old27 results without relabelling them. Recording permission, live readiness and field correspondence remain separate gates. Do not use software passes or runtime liveness to close the unsolved Coaching incident. No further hardware operation or source/binary publication is authorized by this documentation work.

## Historical27 checkpoint

The following27 ledger and off-device investigation retain their original tested source and five-second hold; they are not current28 timing or physical acceptance.

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

S-assisted success is now reported in the path-selection failure situation. It is a useful fallback and a diagnostic distinction, **not acceptance of automatic shaking everywhere**. Touch remains available. Classic remains the startup default; Classic, the canonical catalog and Coaching remain distinct. The27 checkpoint retained the six-second reveal and five-second reading hold (historical timing). Sound remains optional, RAM-only and OFF on boot.

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

See [the focused findings and scoped correction proposal](V3-GESTURE-FOLLOWUP.md). An isolated off-device candidate is now implemented under the approved scope, including Coaching compass automatic shaking; that gesture candidate was not installed. The later28 installation changed timing only. The bounded extension is now independently verified: five contexts expose separate initial-acquisition and post-cooldown-rearm gaps under ordinary synthetic handling; quiet controls recover both and continuous strong motion does not repeat. Independent LVGL-owned inertia also exposes premature UI eligibility, a separate safety gap that cannot explain missing gestures. Short-inertia zero-draw results are not proof of long-inertia safety. The18-case axis/sign/cadence characterization used unequal wall durations and does not establish a cadence defect. Actual human incident attribution remains open; the merged candidate passes its frozen100-control/12-target actual-LVGL fixture, but independent review still blocks wider acceptance on cold slower-sampled cases and proof/cadence timing. Original policy-conflicting cancellation/expiry REDs remain retained; fresh-event equivalence is not manual revival. Real producer-health-to-UI composition now passes35 cases /518 assertions with parent verification and native replay. Design feasibility calls for an explicit automatic-proof ownership decision: freshly completed adjacent opposing pairs would own bounded authorization instead of only the latest raw strong endpoint. That distinct off-device experiment was subsequently approved, implemented and rejected: it fixed the exact26 old cold misses but introduced147 previously working cold misses and lost a healthy-NotReady positive. Parent independent fresh gates confirm primary148 failures and supplemental0 failures. No failed source was merged or installed; no full SDK or installation readiness is claimed. See the follow-up for the preserved comparison and stopping decision.

## Historical27 observer preparation

The53 observer and11 bridge host passes did not establish live readiness. Subsequent actual attempts and the required28 rebinding are recorded above; old results remain bound to27. The synthetic100-second export was a fake-clock result, not a movement trace.

## Next steps — preserve installed28

Keep28 stable; continue the scoped27 investigation off-device without merging its rejected experiment. Any future capture needs28 observer rebinding, fresh consent/readiness and preservation gates. Later source publication requires its own checked, privacy-reviewed scope; this documentation update is not a source backup.

The primary project remains learner-owned, project-led mentoring through **Define → Direct → Check → Adjust**. The device is an optional playful companion, not a curriculum prerequisite, diagnostic instrument or evidence of learning outcomes.
