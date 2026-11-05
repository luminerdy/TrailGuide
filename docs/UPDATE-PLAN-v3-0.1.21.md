# TrailGuide update plan — based on the three v3-0.1.21 reviews

**Original proposal status (historical): implementation had not started when this plan was written.**

P0 and Delivery A software work are now implemented and checked. See the [candidate22 verification and deployment ledger](DELIVERY-A-CANDIDATE22.md) for completed gates and the remaining deployment boundary. The proposal and tracker below preserve their original scope; wording approval and future UI-policy decisions remain separate.

## Recommendation

Proceed in three separately reviewable deliveries:

1. **A — Reliable verification and runtime hardening.** Restore trustworthy checks and fix the reproduced correctness/evidence-preservation gaps without changing the accepted experience.
2. **B — Targeted Coaching-content improvements.** Approve a small wording/guidance sample, then improve the selected records and optional supporting material. Do not rewrite the collection wholesale.
3. **C — Small usability-consistency improvements.** Make Instant predictable across modes and, if approved, restore Classic reading position. Retain the accepted appearance.

Content drafting and the optional operating/mentoring reference can proceed off-device while Delivery A is being checked. The mentoring work must not wait on an unrelated physical flip investigation. However, device integration and release gates remain sequential so a regression has an identifiable cause.

Treat performance optimization, broad refactoring, and optional legacy/browser maintenance as a later, evidence-led backlog—not a prerequisite for every improvement.

## Source reviews and authority

- [Code review](reviews/CODE-REVIEW-v3-0.1.21.md): 29 findings, risks and ideas.
- [UI review](reviews/UI-REVIEW-v3-0.1.21.md): eight consolidated improvement candidates.
- [Coaching-content review](reviews/COACHING-CONTENT-REVIEW-v3-0.1.21.md): ten thematic priorities, supported by complete 55-entry/40-card audits and an 11-reference crosswalk.
- [Complete finding-to-work-package tracker](reviews/UPDATE-PLAN-TRACEABILITY-v3-0.1.21.csv): all 47 top-level review items, including deferred items and their reason/acceptance basis.

The current project/product/V3 specifications remain the implementation contracts. This plan proposes bounded changes to them; it does not silently supersede approved behavior. Reviewer priorities are inputs to planning, not automatic release blockers or implementation authorization.

## Baseline and immediate constraints

- The installed, runtime-verified and qualitatively accepted device baseline is **v3-0.1.21-global-gestures**. Keep it running until an independently checked replacement is ready and a handoff is agreed.
- The local repository has an uncommitted v3-0.1.21 integration plus the review documents. Its HEAD is `a96734776c63f7ee77a7dc4c5c42abb14a9ce507`, the preceding published-source checkpoint. Do not confuse HEAD with the installed payload or discard/stash/reset existing work to manufacture a clean baseline.
- The requested GitHub v3-0.1.21 update has not been completed in this workflow. Resolve that publication separately from new firmware changes; never mix unreviewed fixes into an allegedly exact installed21 publication.
- Available storage at planning preflight was **1,364,725,760 bytes**, below the local **2 GiB minimum reserve**. Heavy tests/builds are blocked until safe headroom is restored, including their expected peak usage.
- A current Python approval-boundary test is known to fail. The latest whole isolated Python run did not produce a completed passing report. Earlier release counts are not a substitute.
- The original intermittent flip/stuck-compass/blank-screen root cause remains **open**. Global start-fresh works well in the reported use; the planned hardening is not a claim that the original defect is diagnosed.

## What stays unchanged by default

- Global intentional flip/shake from owned screens, including reveal/hold: Classic keeps Ask/Try; Coaching draws a fresh opening card and leaves the route. Preserve pointer/scroll suppression, one-event arbitration, quiet acquisition and cooldowns.
- Accepted sensing thresholds/range/scaling, whole-face six-second reveal, five-second reading hold, touch escape and no extra black pause.
- The full wordless gold/blue compass, six freely chosen paths, invisible center Options target, compact card face, fonts, palette, quiet controls and conditional overflow cues.
- Classic availability and boot default; Coaching-first product direction does not itself change startup.
- Output-only Sound, shared RAM preference, default OFF, existing gain/tone and cancellation semantics. Held louder audio is not part of this plan.
- Stable card IDs, linked meanings and existing draft review states. Copy approval and review-state promotion are separate decisions.
- Learner choice, direct help when requested, passing, human-only routes and unrestricted release. No profiling, question quota, compulsory experiment, mandatory transcript or outcome inferred from clicks.
- The device remains optional. The primary aim is better mentoring and learner-owned continuation across varied uses of AI—not a new device-centered curriculum.

## Execution loop for every delivery

Use **Define → Direct → Check → Adjust** consistently:

- **Define:** name one user-visible outcome, preserved behavior, failure/recovery boundary and adequate evidence before changing code or copy.
- **Direct:** implement in an isolated owned branch/worktree or editorial overlay with an exact baseline and bounded file scope.
- **Check:** run the real affected paths, retain actual results, compare source/render/content invariants and obtain one focused independent review of the frozen payload.
- **Adjust:** correct the demonstrated issue, rerun affected checks and update the evidence. Do not restart a broad redesign or an endless review chain.

These are project-working conventions, not required steps that must appear on every learner card.

## Phase 0 — Protect the baseline and recover a usable development environment

**Work package P0 · prerequisite · relative effort: small to medium**

### Actions

1. Freeze an auditable checkpoint of the exact installed21 source, existing working-tree integration, reports, plan and accepted target inventories. Preserve current originals and make rollback inputs explicit.
2. Inventory disk usage and actual process owners. Classify interrupted test sandboxes and caches by ownership—not merely by version-like names. Archive/hash-verify disposable or state-bearing material before removing exact redundant copies. Preserve source, credentials, original flash/vendor inputs, installed roots, target application/ELF/debug maps, captures and accepted evidence.
3. Recheck available space and reserve the expected peak for each job; serialize heavy jobs where necessary. Do not reclaim space by deleting accepted target caches from frozen inventories.
4. Establish explicit test tiers: portable source/content checks; pinned native integration; local immutable-release audit; target compile/package; separately authorized physical acceptance. Document prerequisites and unsupported/optional tiers.
5. Keep publication scope explicit. After the gate corrections below, publish the unchanged installed21 production source and review/plan checkpoint under the existing publication request, with known findings and acceptance limits honestly documented. If a required check remains red, do not claim a verified completion or silently label it green; keep the publication pending or explicitly agree a known-issues checkpoint.

### Done when

- The original accepted source/target inventories are exact and recovery paths are known.
- Storage satisfies the minimum reserve plus the job's estimated peak.
- An interrupted attempt cannot be mistaken for successful verification.
- The installed21 publication payload is distinguishable from all subsequent fixes; no device write is needed to publish source/docs.

**Approval boundary:** approval to execute Phase 0 covers scoped archival/preflight/checkpoint preparation, not arbitrary deletion, changing credentials, firmware installation or rewriting cards. Actual source publication remains its own reviewed/read-back operation.

## Phase 1 — Delivery A: trustworthy checks and runtime robustness

**Suggested next firmware candidate: v3-0.1.22, only once a frozen increment exists.** The version is a proposal, not an installed or built result.

### Q1 — Repair the verification contract

**Review links:** TQ-01, TQ-04, TQ-05, TQ-08, P-04. **Effort:** medium.

- Replace the obsolete whole-header approval boundary with presentation-specific evidence that retains the accepted geometry, typography, full-copy access and relevant native pixels. Do not merely bless the current header hash.
- Give compilation/test children bounded timeouts and expected-event loops; emit useful seed/sample diagnostics on exhaustion.
- Stream per-attempt logs and publish explicit pass/fail/timeout/interrupted status. In cleanup/finalization, record protected-inventory checks and reap owned child/browser process groups before removing a disposable sandbox. A force-killed parent cannot guarantee finalization, so detect incomplete attempts on the next preflight.
- Generate cross-language protocol fixtures from the current writer within the integration tier, bind them to producer/source identity, and require those cases rather than silently consuming stale artifacts or skipping them.
- Make browser-study tests check delivered artifacts unchanged and write synthetic regenerated fixtures only to scratch; do not repair tracked output during test setup.

**Acceptance:** the original compact-boundary failure is explained and replaced by meaningful guards; a clean/current full Python invocation completes with its real counts and no unacknowledged required-case skips; native runs and timeout/failure-injection cases produce trustworthy final status; tracked sources and immutable roots remain unchanged by tests except explicitly owned disposable outputs.

### Q2 — Make preparation and input handling fail safely

**Review links:** TOOL-01–TOOL-04. **Effort:** medium.

- Reject symlink escapes at actual mutation destinations using an independent contained anchor; validate protected roots before any delete/extract operation. Test intermediate symlinks in harmless isolated trees.
- Prevalidate inputs and build a staging tree with an explicit completion/transaction protocol. Preserve the prior complete output if extraction/rendering fails; never leave an old success manifest describing partial output.
- Bound native revisions to the supported integer range and reject unsupported control characters before generating C++ literals. Preserve deliberate punctuation/newline policy and readable author errors.
- Close a serial descriptor if construction/setup fails after opening it; verify with mocks, not live port experiments.

**Acceptance:** path-escape and mid-preparation-failure tests cannot mutate protected/outside data; failed preparation has no false completion record; malformed content fails before publication; constructor failure tests show no leaked descriptor/lock.

### R1 — Propagate known sensor faults without retuning shake

**Review link:** VR-01. **Effort:** small to medium.

- Carry the driver's existing poll-health distinction into the actual sketch-to-UI dispatch.
- A known read/raw/clipping fault cancels partial shake intent; ordinary bounded NotReady pauses remain tolerated.
- Keep accepted thresholds, freshness rules, sample conversion, global routing and persistent fired/cooldown latch unchanged.

**Acceptance:** real owned driver/dispatch tests cover a partial burst interrupted by status/raw failures, invalid raw patterns and clipping; neither mode draws until valid quiet rearming and a new complete burst. Retain the NotReady positive, continuous-rattle negative, flip arbitration and timing-boundary coverage.

### R2 — Reject malformed serial framing before destructive commands

**Review link:** VR-03. **Effort:** small.

- Reject embedded NUL and unsupported control bytes for the entire line, discarding through newline without dispatch. Define valid CRLF handling without permitting embedded control bytes to hide trailing input.
- Retain existing strict integer/session/nonce validation and bounded input budgets.

**Acceptance:** inject controls at each relevant command offset and across input-budget boundaries. Frozen header, row bytes, CRC and session remain unchanged; no CLEAR/REC/FREEZE/DUMP action or allocation is earned by malformed framing. Execute entirely with synthetic transport/data—do not arm the physical recorder for this proof.

### R3 — Reproduce and close the unconsumed sound-event window

**Review links:** VR-02, TQ-07. **Effort:** medium.

- First compose actual UI/LVGL settlement, actual sketch ordering and the host audio adapter to reproduce old settlement → replacement motion draw → audio service in the same iteration.
- If reproduced as described, make unconsumed cue ownership cancellable by draw generation without decrementing shared counters or replaying stale events.
- Explicitly preserve the separate already-playing-worker policy, mute consumption, busy-drop behavior, monotonic event identity, sticky OFF cancellation and bounded cleanup. No gain, frequency, envelope or loudness change.
- Add one economical raw-register shake → real dispatch → active UI → audio-adapter integration path rather than relying solely on separately green components.

**Acceptance:** a replaced card's unaccepted cue cannot start; the replacement earns exactly one cue when readable; quiet boot/navigation/mute/fault paths remain quiet as specified. Both owners, Instant/animated paths and deadline crossings are covered. A non-reproduced hypothesis is investigated or reclassified—not “fixed” through an unrelated redesign.

### Delivery A exit

- Required source/content tests, actual native suite, preserved render/pointer/ownership checks and actual target compile/package pass on identified current source.
- All observed command/test/frame counts are recomputed, not copied from historical 66/86/533 figures.
- Findings covered by the increment have explicit before/after tests; no false claim that the original intermittent physical issue is solved.
- One independent review approves the exact scoped code/package/helpers. Accepted21 remains the fallback.
- Source/doc publication and device installation are reported separately. Installation waits for a coordinated handoff; runtime readback is not physical usability acceptance.

## Phase 2 — Delivery B: targeted Coaching-content and supporting-material update

**Editorial drafting can run alongside Phase 1; canonical edits and device integration wait for approval. Relative effort: medium.**

### C1 — Start with a small exact before/after candidate

**Review links:** CR-01–CR-05, CR-07–CR-08, UI-05.

Recommended first visible-wording candidates:

- `AI-RESULTS-Q02`: compare expectations neutrally instead of assuming disappointment.
- `PROGRESS-E03`: let evidence change confidence, not merely strengthen it.
- `TEAM-Q04`: invite a person's perspective without a proxy claim about everyone.

Recommended first private-guidance candidates:

- `STUCK-E01`: a safe reversible example, an observation/stop boundary and a limited conclusion.
- `EXPLORING-E01`: an optional realistic human-only/AI/hybrid comparison, including full effort and learning—not a requirement to try AI.
- `AI-RESULTS-E02`: one claim, an appropriate evidence basis, and permission to retain uncertainty.
- `TEAM-E01`: voluntary quiet reflection/sharing with no collection or agreement inferred from silence.

These four records import invitations rather than complete canonical activities. Prefer local Coaching guidance when that is the intended scope. Do not change a shared canonical activity assuming it will update these faces.

For each candidate provide exact original/proposed prompt and complete guidance, source IDs/references, rationale, preserved qualifications and downstream consumers. Add a specific listening/checking cue by selective replacement, not automatic accumulation of boilerplate. Keep direct-help/decline/human-only/evidence protections accessible on standalone entry.

Review OPEN-03/05/06, conditional useful-example questions and early closings for applicability next; adaptation/skip guidance may be enough. Keep the other sound records unchanged. A “clarify” note in the audit is not a mandate to rewrite that card.

**Approval gate:** select the exact first wording set before modifying canonical JSON or generated assets. Review states remain draft unless separately and explicitly approved for promotion.

**Acceptance:** prompts work in appropriate fresh/good-output/individual/team contexts without fabricated history or consensus; selected guidance gives useful method support; full meaning/permissions remain; source references resolve; generator checks pass; long/short actual native views retain readable type and complete-copy access. Text length alone is not a fit or effectiveness test.

### C2 — One optional bridge to operation and independent mentoring

**Review links:** UI-01, UI-02, UI-06, CR-06, CR-09.

Create a compact off-device reference with two clearly separated purposes, potentially two sides/pages:

- **Operating side:** touch-only entry, center Options, global start-fresh versus Another/Opening/Back, same-card tap escape, Instant, complete-copy scrolling and least-destructive recovery limits.
- **Mentoring side:** six freely selectable contexts; Define → Direct → Check → Adjust with checking planned early; requested help styles; proportional AI/tool-assisted checks; full-effort human/AI/hybrid comparison when useful; the existing optional learner-owned continuation brief.

Reuse current material in `PROJECT-COACH.md` and the flow documents. Include a small number of writing/research/creative/organization examples so CLI coding is one optional context, not the assumed curriculum. No mandatory onboarding, extra permanent chrome, question quota, compulsory form, transcript or participant scoring.

**Acceptance:** a reader can distinguish return-to-compass from new-opener, find a relevant checking/help route, and retain only useful continuation context. Reuse completed observations and hypothetical examples first; request a small voluntary real-use check only if a material uncertainty remains.

### C3 — Apply shared-field governance and content regression checks

**Review link:** CR-10. **Effort:** small ongoing discipline.

Use the 11-reference crosswalk before edits. Preserve IDs and distinct 40/12/55 resources. Check paired Ask/Try/Guide meaning, conditional follow-ups, optional AI help and the ten optional Agent Workflow adaptations. Increment existing canonical revisions when appropriate to approved semantic changes; do not invent a new Coaching revision field or change schema/review states casually.

**Acceptance:** every changed field has an explicit consumer list; only approved fields/revisions change; generated artifacts match; no qualifier is lost through reuse/normalization. Publication/build/install remain distinct later steps.

### Delivery B exit

- Exact editorial diff approved, limited to the chosen sample and support material; untouched records remain byte/meaning stable as applicable.
- Native preview of changed faces and guidance includes short/long text, full-copy endpoints and preserved layout.
- Technical tests establish correct content delivery; they do not claim improved learning or field effectiveness.
- Produce one separately identified content candidate after Delivery A's baseline is stable; do not bundle an unrelated UI redesign.

## Phase 3 — Delivery C: small consistency changes, not a visual overhaul

### U1 — Shared RAM-only Instant, if approved

**Review link:** UI-03. **Recommendation:** share one preference across modes, matching the conceptual scope of the existing Sound preference. **Effort:** small to medium.

Remove surprising one-way overwrite while preserving the fresh-boot default, lack of persistence, six-second default animation, five-second hold and decorative compass behavior. Approval of shared ownership is required; the review did not establish it as an existing requirement violation.

**Acceptance:** real native pointer tests toggle in either mode, exit/re-enter, and compare displayed/applied state in both directions. New boot resets as specified. No new cue, stored setting or timing change appears.

### U2 — Classic same-card reading-position memory, if approved

**Review link:** UI-04. **Recommendation:** retain separate main/Guide offsets for the same card identity and Ask/Try face. **Effort:** medium.

Reset positions on a genuine new draw; clamp to valid content bounds; do not add persistent history, a Home resume action, restart undo or extra controls. Preserve Coaching's current per-face memories.

**Acceptance:** long-card Guide/Back restores the intended point without RNG/draw; changed identity/mode/body cannot inherit an unrelated stale offset; global new draw starts at top; scrolling and More cues remain truthful.

### U3 — Validate before any geometry/palette/timing proposal

**Review links:** UI-07, UI-08; UI-05/06 regressions remain covered in content/native checks.

Keep current visuals. If later ordinary-use evidence shows a problem, distinguish glare, font size, semantic confusion, grip occlusion and target-boundary behavior. Use intended grip/real brightness under permitted conditions; no deliberate wetting or unsupported glove test. Preserve invisible hit areas, pressed/focus feedback and safe-circle masking.

**Acceptance for considering a visual candidate:** an observed problem, a narrowly scoped proposed change and a same-conditions baseline/candidate comparison. Native pixels are not physical contrast or touch-size proof. No field-readability certification follows from this plan.

### Delivery C exit

Only approved consistency items ship. No compass redesign, brighter controls by default, boot change or faster default reveal. If real use does not justify additional visual work, close that branch of the backlog as “retain current design.”

## Phase 4 — Conditional engineering and legacy backlog

### D1 — Make development checks portable and predictable

**Review links:** TQ-02, TQ-03, TQ-06, TQ-09. **Effort:** medium; split as useful.

Publish a prerequisite/preflight manifest and separate portable checks from machine-local preservation audits. Make cache creation/full-suite escalation explicit and resource-guarded. Bind cache/replay artifacts to compiler/target/configuration/library/source provenance; support safe temp/copy behavior across filesystems. Report missing prerequisites precisely rather than silently performing unrelated preparation. Small prerequisite reporting can land with Q1; broader portability need not block an otherwise validated device fix.

### D2 — Measure memory and scheduling before optimization

**Review links:** VR-04, VR-05, VR-07. **Effort:** medium, conditional.

First test actual object-heap pressure/fragmentation separately from caller-raster or null-timer fallback; make Classic compass allocation checks consistent where justified. Measure renderer, refresh/flush, actual poll gaps and full recorder freeze separately before choosing an optimization. Consider incremental CRC or skipping invisible projection only if measurements justify it and exact ABI/pixels/wall-clock behavior remain correct.

Target measurements require separate bench authorization and a coordinated device handoff. No promised FPS, memory endurance or battery improvement. A serious reproduced allocation-safety failure is promoted into the active reliability increment rather than ignored because it was initially a risk.

### D3 — Improve code readability after behavior is locked

**Review link:** VR-06. **Effort:** medium; defer broad extraction.

Expand dense lifecycle code into named, readable steps in a separate behavior-preserving change. Consider a narrow presentation-session helper only if duplication demonstrably warrants it. Keep source review, both-owner deletion/cancellation tests and native raster comparison. Do not combine a large refactor with a fault correction or editorial release.

### L1 — Maintain optional legacy/browser surfaces only as needed

**Review links:** LEG-01–LEG-04, ARCH-01, P-01–P-03.

If V2 is actively reopened, address conditional label-lifetime hazards, bind model/content counts and measure target-specific heap behavior. If old S3 bring-up is deliberately reused, carry its own serial-backpressure qualification; do not deploy it as a repair for current V3. If historical browser studies remain supported, fix focus restoration, clearly mark or complete reduced guidance, and test qualified touch-release ordering. Share narrow contracts/test vectors before consolidating target implementations.

These are not demonstrated current-V3 failures. No legacy reflash, networking expansion, dependency upgrade or generic cross-target firmware rewrite is in the active delivery scope. P-04's artifact-mutating tests are handled earlier in Q1 because they affect verification integrity.

## Common release, publication and recovery gates

For each approved firmware delivery:

1. **Source gate:** exact reviewed scope, current source/input hashes, preserved IDs/content/geometry/policy outside the increment; no secrets or private captures in public payloads.
2. **Execution gate:** meaningful RED before the fix where applicable; current required tests/native integration and actual S3 package validation; retain failed attempts and recheck completion status. Required-case skips need explicit disposition, not a hidden green label.
3. **Review gate:** one focused independent verdict over the frozen code/content/package/helper payload. Address local blockers and rerun affected checks; no endless re-review of unchanged work.
4. **Hardware handoff:** ask once when the replacement is ready. Identify the exact target and port ownership; preserve valuable RAM evidence before serial opening/reset; application-only transfer with compatible components and known recovery image.
5. **Readback gate:** explicit flash comparison, exact fresh-ELF version, initialized active UI and advancing valid real IMU/application counters; resume/detach. Intrusive probes do not count as natural gesture or pixel acceptance.
6. **Human gate:** one focused observation for the changed experience; retain prior approvals. No mandatory exhaustive trials, recorder activation, speaker bench replay or cleanup reset. Leave the working state running after success.
7. **Publication gate:** update affected specs/flows/handoff and unresolved limits; audit the complete unpublished blob range, push only explicit reviewed paths without force, then verify the exact remote commit and changed-file contents. Record source publication separately from installation/physical acceptance.

Keep accepted21 application/ELF/maps and other installed inventories immutable. A failed transfer or live check triggers state inspection, not fabricated success, repeated blind flashing or an assumed automatic rollback. A recovery write is an explicit controlled operation, not background cleanup.

## Approval checkpoints and boundaries

- **Now:** approve or revise this proposed scope and order. Creating this plan does not authorize implementation.
- **Start work:** recommend authorizing Phase 0 plus Delivery A, while allowing an off-device C1/C2 draft overlay in parallel. Drafting does not apply the wording.
- **Content:** approve the exact before/after sample and complete guidance/consumer scope before canonical edits.
- **UI consistency:** approve shared Instant and same-card Classic scroll behavior before implementation. No visual or boot change is implied.
- **Device:** separately agree installation readiness when each tested candidate is ready, especially if the user is using/testing the current build.
- **Performance/legacy:** authorize only where evidence or actual continued use justifies it.

Relative effort estimates describe scope, not calendar promises or verified speedups. Version labels for later content/usability candidates should be assigned when their payloads are frozen. This plan makes no changes to the accepted device and makes no claim that any listed update is already implemented, installed or published.
