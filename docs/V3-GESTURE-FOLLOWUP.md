# V3 gesture follow-up — installed27

## Status and scope

**Investigation, not an implemented fix.** Installed `v3-0.1.27-accel16` remains unchanged. Physical reports establish a useful improvement but leave Coaching compass shaking and intermittent automatic shaking after path selection open. S followed by shaking works in the reported path-selection failure situation.

The committed firmware baseline remains `v3-0.1.22-reliability`; this document does not publish or integrate the private27 firmware, recordings or deployment tooling. See the [progress ledger](V3-PROGRESS.md), [delivery plan](PLAN.md) and [verification record](BRINGUP.md).

## What the off-device comparison demonstrated

The exact installed27 build-bound source was exercised through actual LVGL input and UI/model dispatch with ASan/UBSan. These are synthetic scenarios, **not a replay of the unrecorded27 human movements**.

- Across all six real pointer-selected Coaching paths, an immediate automatic burst without renewed quiet acquisition produced no event or model draw.
- The identical burst succeeded after renewed quiet acquisition, or after a real released S tap. Each successful case produced one event and one model draw in the tested fixture.
- Classic compass, Classic card, Coaching compass through normal UI delegation, and Coaching opening-card positive controls passed after acquisition. No unconditional Coaching-compass dispatch prohibition was found in those tested paths.
- Contact and navigation reset automatic acquisition. Automatic recognition then requires fresh quiet/reference acquisition; S uses a different bounded seed path. Coaching and Classic own separate recognizer instances, so their prior acquisition histories can differ.
- Pointer contact, pointer-driven scrolling, fault/stale/gap cancellation, recovery and continuous-episode controls passed within their documented scope. Independent post-release inertia and broader rearm coverage remain separate checks.

The focused native packet contains 27 scenario rows. Its mechanism/control run exits successfully; its separately labelled unchanged-baseline contract run intentionally remains RED. The parent re-executed both from source/fixture/binary-bound evidence and verified the six-path comparisons. Actual-header probes also demonstrate sensitivity to translation direction and fresh-frame cadence; they do not identify the conditions of the human failure.

**Interpretation:** a deterministic post-navigation no-stillness requirement gap is demonstrated. It plausibly fits the S-recovery pattern, but it is not proof that absent quiet caused the physical failures. It does not by itself explain why the human Classic compass worked while Coaching compass did not. No attributed27 movement trace exists.

Quiet-frame positives are diagnostic controls, **not a new instruction to hold the device still before using it**. The intended experience remains a natural deliberate burst while ordinarily handling the device.

## Ranked explanations and unresolved evidence

1. **Automatic acquisition reset after contact/navigation:** strongest demonstrated mechanism. A burst alone cannot reacquire in the tested baseline; S takes another acquisition route. Actual per-attempt reference/quiet history remains unknown.
2. **Input/scroll ownership or repeated reset:** possible contextual contributor. A persistently stuck state was not reproduced in the tested routes. A later S tap also changes elapsed time and contact state, so S success alone cannot isolate this cause.
3. **Direction, magnitude, phase or sample-credit cadence:** supported as implementation sensitivities, not attributed incident causes. No blind threshold or duration change follows from these probes.
4. **Prior fired latch/cooldown or health interruption:** real suppression mechanisms with passing bounded cancellation/recovery controls; no field history establishes their involvement.
5. **Coaching-specific dispatch/lifecycle defect:** lower support after delegated native compass/path positives, but untested histories remain possible.

A separate premature-input-enabling defect, if found during inertia checks, would have the opposite failure direction from permanently missing shakes and must not be presented as the explanation for this incident.

## Proposed correction boundary — approval required

The proposed direction is to investigate **bounded automatic acquisition from a genuine alternating burst after navigation/contact**, without requiring a quiet setup interval. This is a design proposal, not an approved policy candidate.

- Establish a fresh provisional gravity/reference from bounded motion evidence; require confirmed opposing burst evidence before crediting a draw. Explore whether suitable evidence rules from the reviewed manual seed path can be reused without granting manual intent automatically.
- Clear partial credit on contact, scrolling, faults, stale data and excessive gaps. Do not carry a partly completed pre-navigation gesture into a new context or reuse an unvalidated reference indefinitely.
- Preserve one draw per continuous episode, cooldown/rearm protection, global Start Fresh semantics and the touch/S alternatives. Navigation itself is not consent to draw a card.
- Preserve the ±16g configuration/conversion/safety correction, gyro configuration, flip behavior, accepted UI/copy and reveal/hold timing. Do not use threshold lowering or a longer intent window as an unexplained substitute.
- Compare retaining only a still-valid reference across benign scene changes as a narrower alternative. That alone would not address cold-start acquisition or justify retaining partial motion credit; choose only after focused controls distinguish the cases.

Before implementation, complete the bounded off-device acquisition/rearm and independently observed scroll/inertia comparisons. Freeze working compatibility cases and new no-stillness requirements separately, then present the smallest supported change and validation matrix for approval.

## Validation and stopping gates

A future approved candidate must pass the unchanged six-path quiet/S positives, the no-quiet target rows, Classic and Coaching compass/card controls, and actual pointer/scroll/fault/stale/gap and one-episode negatives. Extend axis/onset-phase/cadence and ordinary-handling controls rather than claiming one stepped waveform represents natural shaking. Missing data and changing references must not accumulate stale credit.

Focused tests, independent review, applicable full native/target/package checks, installation permission and live readback/runtime verification remain distinct from physical acceptance. A later short, separately approved and prepared attributed recording is needed to distinguish actual27 incident causes. No further movements are requested while human testing is paused.

The initial investigation's native linking exceeded its16 MiB working-output peak budget; those failures remain recorded and are not called clean resource passes. The final source-bound replay and the parent's independent reruns stayed within that replay budget. Future fresh compilation needs a separately measured envelope that includes objects, uncompressed sanitized binaries and compression coexistence; a small retained archive does not establish a small linking peak. These are workspace-file footprint checks, not process-memory measurements.

**Device boundary:** no new firmware, reset, recording or installation is authorized by this investigation. Preserve the working27 setup and report what remains uncertain.
