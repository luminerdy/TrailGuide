# Delivery plan

## Installed27 / published-source checkpoint

**The development device runs `v3-0.1.27-accel16` DRAFT.** Full application readback and exact-version runtime checks passed. Shake now works in reported Classic compass/card and Coaching card contexts, but Coaching compass shaking fails and automatic shaking is intermittent after path selection; S then shake works there. Physical acceptance is partial, not a reliability or exactly-once result.
See the [current progress and verification ledger](V3-PROGRESS.md) for the installed scope, prior physical failures and remaining checks.

The committed firmware baseline remains `v3-0.1.22-reliability`. This documentation checkpoint does not integrate or publish the later private source snapshots or firmware packages. Earlier release descriptions below retain their historical scope; they are not current27 physical acceptance.

## Next steps — Coaching shake reliability

Physical testing is paused; keep27 installed and leave the device unchanged. Off-device implementation and testing are approved, explicitly including automatic Coaching compass shaking. Installation is not included.

1. **Completed:** compare installed contexts, automatic versus S-assisted acquisition, real pointer/scroll ownership and Start Fresh dispatch. No27 movement trace exists; synthetic failures are not incident attribution.
2. **Completed, scoped:** implement the isolated three-header overlay and run the independently frozen actual-LVGL comparison. All100 controls and12 mandatory targets pass, including Coaching compass entry/return, all six paths, smooth shaking and ordinary-handling rearm. Independent review confirms those results, not broader release readiness.
3. **Current behavior gate:** resolve design feasibility for the cold slower-sampled reference-admission and separate proof/credit timing mechanisms before spending the final bounded refinement. Do not lower thresholds, extend strong-proof age silently or credit missing evidence. Stop blocked if the remaining scope cannot satisfy the requirements.
4. **Current coverage gate:** add separately named policy-updated producer → health-expression → actual-UI tests. Retain old cancellation/expiry REDs and prove that a later full automatic burst is a new event, not revived manual intent or retained credit. Preserve current-range fault, NotReady, stale, duplicate and continuous-episode safeguards.
5. **Only after required behavior passes:** complete independent review and applicable full native/target/package gates. Preserve the ±16g gain, accepted appearance, flip implementation, global Start Fresh and touch/S alternatives. Installation/readback/runtime and later physical acceptance remain separate.
6. When human testing resumes, coordinate one short, separately approved Coaching-focused recording. No more movements are requested during the pause; do not claim physical repair from the native fixture.

See [V3-PROGRESS.md](V3-PROGRESS.md) and the [implementation/review detail](V3-GESTURE-FOLLOWUP.md). This documentation publication excludes private firmware source/packages and unrelated editorial work.

### Scoped implementation approved; installation remains gated

The [focused gesture follow-up](V3-GESTURE-FOLLOWUP.md) distinguishes the reproduced acquisition/rearm gaps, the separate scroll-safety correction, approved off-device implementation and its current review blockers. The private merged candidate passes the focused Coaching compass/path contract, not the complete field-reliability goal. No device operation or publication of private firmware source is implied.

## Project delivery scope

Goal: develop mentoring for individuals and teams learning through meaningful projects with varied AI assistance, and help learners continue with suitable support rather than depend on the mentor. The physical device is an optional, enjoyable building/novelty track; Frontier Ops remains a reusable theme. Preserve V1 and the running V2 trial.

Canonical requirements: [Living product spec](PRODUCT-SPEC.md). Two-mode implementation handoff: [Ask the Team / Try a Move](FACILITATOR-MODES.md). Proposed circular UX and prototype gates: [ROUND-UI-SPEC.md](ROUND-UI-SPEC.md). Hardware contracts: [V1 OLED](V1-SPEC.md), [V2 portrait C6](V2-SPEC.md), [V3 round S3](V3-SPEC.md). Execution evidence: [BRINGUP.md](BRINGUP.md). Planned work is not implemented functionality.

## Historical implementation baseline

Current installed27 progress and the remaining physical gates are tracked in [V3-PROGRESS.md](V3-PROGRESS.md). The following baseline description records earlier releases, not current27 physical acceptance.

**Approved direction: Coaching-first, with revised V3 wording as its content foundation.** This is not a startup-default change or a content merge: Classic still boots by default and remains an intact optional quick-prompt route. Coaching currently uses its own authored layer plus canonical references; neither all twelve Classic pairs nor the whole forty-card catalog has been merged into Coaching.

**v3-0.1.21-global-gestures DRAFT is installed**. This remains an offline authored-content trial, not a production release; local installation does not imply publication. Classic remains the boot default and its twelve-card Ask/Try experience is intact.

Selecting **Coaching** now enters a wordless decorative compass with the full gold/blue arrow and six labelled path icons. The model silently selects or retains its opener; entry does not show the question. The invisible **136x64 center** opens Options. Decorative wandering uses an independent LCG and never selects a path or consumes a card draw. **Another opener**, or a completed intentional turn over and back or sustained shake from any owned Coaching screen, displays the retained blue-card reveal (six seconds plus five seconds of reading hold). **Opening** from a path returns to the compass without changing the saved opener; Options **Back** restores the compass or opener-card variant it came from.

The installed **v3-0.1.21-global-gestures** S3 application is **691312 bytes**. Release evidence includes 12 global-policy cases and 14 actual-LVGL integration groups under ASan/UBSan and warnings-as-errors, actual S3 compilation and independent scoped review. Application-only flash comparison, exact live Version, initialized active UI and advancing valid real-IMU readings passed. This documentation update does not contact hardware. Historical v3-0.1.20 evidence remains separate: 42 retained detector assertions, 26 rotation-policy assertions, 14 actual-LVGL integration groups, strict/sanitized checks and independent review; its canonical native suite, 86 isolated Python checks and 533 archived native frames are not new v3-0.1.21 results. The earlier 84-check/512-frame evidence belongs to v3-0.1.18.

The installed global start-fresh change is **qualitatively user-accepted in both Classic and Coaching**. This does not establish a trial count, exhaustive per-screen/per-gesture coverage or sustained field reliability. Earlier Coaching-compass shake and UI-cleanup acceptance remain historical evidence. Exhaustive readability/touch, Sound menu/mute/chime, uptime and battery acceptance remain separate. **The original intermittent turnover-stuck-compass/blank root cause remains OPEN**; global policy acceptance is not a diagnosis or proof of its repair.

The [project specification](PROJECT-SPEC.md), [Classic flow](CLASSIC-FLOW.md) and [Coaching flow](COACHING-FLOW.md) describe what exists; [HANDOFF.md](HANDOFF.md) and [V3-TRIAL.md](V3-TRIAL.md) hold evidence and acceptance limits. Team Trail, theme filtering and durable storage remain planned, not shipped. Local installation/docs do not imply publication or authorize further firmware changes.

A completed intentional turn over and back or a deliberate sustained shake starts fresh from **any owned screen**, including Options, Guide, paths, questions, experiments, the path picker, closings, reminders, the Opening compass and an active reveal or reading hold. Classic draws once in its active Ask/Try mode, including after Home, and clears Options before showing the card. Coaching leaves the current path, clears route history and draws one fresh **Opening card**, not another question in that path or a return to the compass. Old reveal/hold resources and pending settlement cues are canceled: no old callbacks, queued draws or stale cues reach the replacement. Actual pointer contact and inertial scrolling still suppress motion. Partial gesture intent and qualified blackout survive autonomous same-card reveal settlement and hold deadlines. Turnover stays black while qualified inverted and starts the new reveal on completed return; shake goes straight to reveal without an extra timed black pause. Each new card retains the six-second whole-face reveal plus five-second reading hold. One sustained burst draws once; continuing to rattle cannot queue another card. Recognition retains 400ms of fresh quiet, 900ms of credited fresh motion and ten opposing peaks, then a 2200ms cooldown followed by fresh quiet to rearm. These are safeguards, not a prescribed handling ritual.

Classic no longer shows the constant ASK/DRAFT/flip footer; canonical review states remain draft. Classic **More text** and the Coaching scroll hint appear only when actual laid-out text remains below the visible viewport, including the opener’s 14px bottom clipping mask—not merely because padding permits scrolling. Mode centers, Options targets, six path controls, content, typography and reveal/hold timing are otherwise retained.

## Primary track — project-led mentoring and independence (P-19)

Status: endorsed direction; draft [mentoring practice](COACHING-DIRECTION.md) and a [learner-owned project/coaching brief](PROJECT-COACH.md) are available for review. Research notes and a bounded synthesis have been collected separately; a reviewed curriculum, reliable integrated AI coach and effectiveness claims remain unestablished.

1. Start with a learner's chosen project and next useful milestone. Connect to AI Learning Lab's Define → Direct → Check → Adjust and flexible AI-role map; do not create a mandatory ladder or competing curriculum.
2. Rehearse one mentor skill with a fictional example, then use an appropriate explanation, invitation, demonstration, collaboration or direct help with permission. Cover creation, research, analysis, learning, planning, coding and collaboration—not only verification or hesitation.
3. Try one small piece, inspect what happened and adjust. Invite voluntary feedback about usefulness and pressure, without profiling, scores or AI-adoption targets.
4. Offer a learner-owned continuation brief for their next AI session or human conversation. Make help style, evidence, uncertainty and a next step explicit; sharing remains optional and privacy-aware.
5. Review evidence and observed usefulness to select one small improvement to the materials or optional tool. New AI services, automated role-play, tracking and practice interfaces need separate scope/tests; a prompt template does not implement them.

Exit for an increment: usable material and an actual project step exist, review feedback/limitations are explicit, and the learner can choose how to continue. This is a design/review goal, not a claim that learning or independence has been measured. Do not require forty-card enrichment, storage or hardware completion to begin. Existing device safety/release gates still apply whenever those capabilities are claimed.

## Optional companion track — hardware and curated cards

The numbered phases below are device delivery gates, not the project's main curriculum or prerequisites for mentoring practice. Browser experiments can proceed independently; physical ports remain gated by the actual hardware and relevant acceptance checks.

## Phase 0 — Finish and preserve V1

Status: in progress. v1-0.2.4 SH1106 display and mechanical BOOT behavior are user-confirmed; full acceptance is not complete.

- Finish fifty-press/thirty-minute, full-deck readability, restart and independent-power checks.
- Record human observations separately from serial/host-test results.
- Preserve known working firmware, credentials and USB recovery instructions locally; publish source/docs only.
- Keep V1's twelve-card whole deck stable while V2 develops.

Exit: V1 acceptance evidence recorded, recovery usable and all remaining limitations stated.

## Phase 1 — Content and workflow design, before V2 integration

Status: forty-card draft catalog and eight representative paired faces authored; content-contract tests pass. An initial read-only round browser preview is built and scripted/browser checks pass. Facilitator editorial/interaction review and production runtime integration remain pending.

- Stable IDs/revisions are assigned to all forty draft cards, preserving the twelve existing titles/actions without changing V1 firmware.
- Eight themes have five cards each in [CARD-CATALOG.md](CARD-CATALOG.md), sourced from [content/cards.json](../content/cards.json). Ten existing cards also have draft agent-workflow variants with Codex/Claude Code notes and human checks, indexed in [AGENT-WORKFLOW.md](AGENT-WORKFLOW.md). No new theme or cards are added. Complete facilitator editorial review before inclusion.
- First author eight representative draft paired faces, one per theme, using the IDs in ROUND-UI-SPEC.md; preserve existing base actions and agent variants. Define separate Ask and Try coaching/review scopes; do not silently repurpose `question` or pretend it is already approved Ask content.
- Build a read-only eight-card round prototype before expanding the paired catalog or writing storage migration. Test Home, main card, one Guide me entry, linked faces, paged actions/themes, synthetic team context, empty eligibility and a long-text fixture. Record/save are visibly disabled; Team Trail is unavailable or a clearly static preview. Use the proposed 360x360 safe-circle and font budgets, then review actual task completion.
- After read-only review, design production No team, saved-vs-used, team/mode confirmation, stale/double-tap handling, Undo, empty trail and write-failure states. No fake persistence or success messages in the preview.
- Define synthetic fixtures and tests for content/deck/use-event/storage logic.

Exit: facilitator review of navigation and representative cards is complete; stable data contracts and acceptance tests are defined. Prototype approval does not establish hardware readiness.

## Phase 2 — Current V2: portrait ESP32-C6 bring-up

Status: C6 revision 0.2 and 8MB flash are probed; two full original-flash reads match. The manufacturer demo was reported working. An initial read-only TrailGuide portrait application, v2-0.1.8 DRAFT, now runs on C6 with eight pairs, Ask/Try, Guide me and ordered Another. Actual-LVGL host tests and synthetic on-device navigation passed. Human TrailGuide screen/touch review, exact PCB identity and sustained memory/performance remain open. See [V2-TRIAL.md](V2-TRIAL.md). Filtering and tracking remain unimplemented. Authenticated V2 Wi-Fi/OTA, wrong-password rejection, partial-transfer recovery and two network updates are verified under [V2-WIFI-UPDATES.md](V2-WIFI-UPDATES.md); this trial is not Phase 2/3 release acceptance.

- Identify exact C6 board/revision, display/touch drivers, pins, memory, partitions and storage budget. Retain the original firmware if readable before an authorized write; keep V1 excluded from C6 commands.
- Run the correct C6 manufacturer display/touch examples before integration; verify any optional sensors independently. Review a separate 172x320 portrait layout/preview rather than scaling the existing circle.
- Pin actual working toolchain/dependencies; retain compatible authenticated OTA and USB recovery.
- Build independent V2 target with shared tested content/deck logic, readable card view and tap-to-draw.
- Keep touch-first controls and idle/wake behavior simple. The v2-0.1.8 text reveal is a software effect with same-card Reveal all and instant Guide/Back; it does not require an IMU. Keep actual shake deferred until the relevant sensor and input safeguards are verified; suppress accidental queued actions and review physical pacing/readability separately.

Exit: core touch/display path works on actual hardware with evidence. No team-history release until durable storage passes Phase 3.

## Phase 3 — Core release: themes and Team Trail

Status: planned. Shared requirements are P-01–P-07 and P-10–P-16 in PRODUCT-SPEC.md; P-18 applies to current V2 portrait and P-17 to future V3 round. V2 release does not wait for V3 hardware acceptance.

- Ask/Try entry without setup; Ask defaults on fresh sessions; optional eight-theme filtering; core Guide me in both modes.
- Mode-aware eligibility/pool tests, linked-face behavior and card revisions. Integrate reviewed paired content without duplicating the catalog.
- Small preset team roster and persistent active-team indicator.
- Mode-specific Asked this / Offered this move confirmation with actual Ask/Try mode and full team identity, deduplicated durable event, Undo and optional feedback. Test legacy-mode migration before enabling history.
- Team-scoped saved ideas; personal saves when No team is selected.
- Team Trail: recent uses, separate saves, themes explored and last tried prompt.
- Recoverable bounded storage, honest event ordering without an assumed real-time clock, schema/version safeguards and explicit full/error states.
- Validate privacy, accidental input prevention, attribution, reboot, power-interrupted writes, migration and corruption behavior with synthetic data.

Exit: all applicable product-spec acceptance checks pass; actual-device screen review, retained-trail reboot, one-hour USB soak and V2 OTA/data-retention checks recorded. No real team data in GitHub.

## Phase 4 — Depth and follow-through, only after core

Status: planned, dependent on core acceptance.

Priority order:
1. Additional Steps and AI prompt pages inside the single Guide me view. Core listening/activity guidance and follow-ups are required by P-16; no separate Show me how entry.
2. Dismissible follow-through with Try again / Adapt it / Move on.
3. Situation shortcuts, personal favorites and optional Agent Workflow collection selection/tool-note panels. Preserve base card identity; variant attribution is a separately tested storage enhancement.
4. Explicit local export/backup and restore tool; decide before workshop collection whether proceeding without backup is acceptable.

Each addition needs its own tests and screen/readability check. Defer features rather than compromise Phase 3 reliability. Free-text notes, live AI, cloud sync, audio and analytics are out of the first release.

## Phase 5 — Freeze, rehearsal and workshop readiness

Acceptance-gated sequence:

- Freeze scope once core acceptance passes; choose tested V2 core or V1 fallback based on evidence.
- Stabilize with fixes, facilitator rehearsal, power/runtime and recovery checks.
- Release for workshop use with a known working fallback and explicit limitations.

Power/case work requires verified connector, polarity, charging requirements and fit. Measure actual runtime/charging with final firmware before claiming battery readiness. Retain USB cable/power option, source, local recovery builds and V1 fallback.

## Research and editorial review within the primary track

Status: draft materials and separately collected research inputs exist; human review, any pilot and additional interfaces remain pending. See [COACHING-DIRECTION.md](COACHING-DIRECTION.md) and the [current handoff](HANDOFF.md).

- Review original research on learning, mentoring, invitation, feedback, accessible participation and appropriate AI reliance. Distinguish findings, guidance and hypotheses, domain/transfer limits and contradictions. Collected notes do not establish TrailGuide's effectiveness.
- Review the draft before/during/after practice and portable coaching brief through real project steps. Include positive creating/building situations as well as uncertainty; the work need not fit a small display. Review the existing eight pairs separately before expanding device menus or the catalog.
- Co-design and pilot with consent, passing and non-AI/access alternatives. Evaluate agreed observable usefulness and voluntary feedback, not inferred learner ability or AI-use counts.
- Select the smallest next content/UI increment after evidence and human review. New practice modes or automated role-play require a separate scope decision.
- Keep this track independent of firmware changes while preserving all existing privacy, content, storage and release gates. No hidden recording, profiling or learning-outcome claims.

## V3 — round ESP32-S3 trial and production gates

The round Waveshare ESP32-S3-Touch-LCD-1.85 is V3. Preserve V3-SPEC.md, ROUND-UI-SPEC.md and the existing full-flow circular browser preview. A separate [shake/reveal browser experience study](V3-EXPERIENCE.md) under P-20 explores the endorsed novelty now: deliberate shake → one random curated question/move → gradual text, with a tap fallback, Reveal all and Instant text. It is a simulation, not IMU firmware, a replacement for Guide me/navigation or a reason to change V2.

The [V3 bring-up checkpoint](V3-BRINGUP.md) establishes Rev2.1/S3/16MB, originals, touch and real IMU identity. The previous [v3-0.1.11 trial](V3-TRIAL.md) was installed and startup/UI/IMU verified; the current v3-0.1.21-global-gestures baseline is above. Twelve endorsed draft Classic openings and guidance remain integrated. The current sustained-shake alternative is separate from the unchanged turnover recognizer and six-second reveal/five-second hold. The prior recovery-lock/logging corrections remain. A bounded RAM recorder retains actual polls without a live reader; complete CRC exports pass. Actual target card draw/reveal/hold passed only under explicit synthetic detector state; one natural turnover now showed blank → card animation → controls with one clean recorded trigger; repeated-use reliability remains open. The measured saturation was addressed with a supported wider gyro range and matching conversion/safety checks, without adding a more complicated gesture. The later presentation-only blackout uses the existing inversion milestone, restores on cancellation and retains the original reveal/hold. V1/V2 remain untouched on hardware.

Before extending the trial toward production, retain the exact S3 board and target-specific display/input/layout/storage boundaries. Verify the IMU, rest/cooldown detection, accidental-handling suppression, responsive nonblocking reveal/cancellation and actual pointer-contact and inertial-scroll gates across all owned views. Review whether the pause is fun and useful, rather than assuming novelty improves learning. V2 results do not verify V3 hardware, partitions, memory or OTA; P-20 does not block the primary track or V2 acceptance.

## Working agreement

Software contributors handle code/docs/tests; hardware operators handle physical wiring and report visible/hands-on results. Update PRODUCT-SPEC.md as requirements evolve, PLAN.md as scope changes and BRINGUP.md with real evidence. Publish verified commits to the configured project origin; never publish credentials, credential-bearing binaries, private logs or real Team Trail data.

## Current implementation summary

V1 has twelve cards, address detection, debounce, shuffled draws, serial fallback and authenticated OTA. Host tests and ESP32 compilation passed; v1-0.2.4 was USB-flashed and serial-verified after correcting the OLED driver to SH1106. User confirmed display/button behavior. Prior firmware verified wireless updates, password rejection, reconnect and interrupted-transfer recovery; OTA code is unchanged. Remaining acceptance is tracked in BRINGUP.md.

The forty-card catalog now contains twelve paired draft entries, used by the native V3 selection. The earlier browser studies and preserved V2 trial still select eight; V2 remains installed with its prior eight-pair review-order runtime. New generated V2 content is for future builds only. All review scopes remain draft, and production eligibility, durable storage and human acceptance remain separate. The product specification describes intended behavior, not a shipped coaching system.

## Immediate project handoff

Use HANDOFF.md for the current checkpoint rather than repeating bootstrap work. Begin with a meaningful project step, the draft mentor practice and optional learner-owned coaching brief; review usefulness and choose a bounded next increment. Try the separate V3 reveal study for its feel, not as hardware evidence. Preserve the v2-0.1.8 portrait trial, authenticated OTA and V1 fallback; human guide/readability/touch review remains pending and separate. Complete paired-content review, production eligibility/pools/events/migration before enabling recording. Align the older round full-flow guidance separately. FACILITATOR-MODES.md and V2-SPEC.md/P-18 remain the device runtime contracts.
