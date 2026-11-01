# Delivery plan

Goal: dependable offline facilitation support for AI workshops using the reusable Frontier Ops theme. Preserve V1 as the fallback.

Canonical requirements: [Living product spec](PRODUCT-SPEC.md). Two-mode implementation handoff: [Ask the Team / Try a Move](FACILITATOR-MODES.md). Proposed circular UX and prototype gates: [ROUND-UI-SPEC.md](ROUND-UI-SPEC.md). Hardware contracts: [V1 OLED](V1-SPEC.md), [V2 portrait C6](V2-SPEC.md), [V3 round S3](V3-SPEC.md). Execution evidence: [BRINGUP.md](BRINGUP.md). Planned work is not implemented functionality.

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

Status: C6 revision 0.2 and 8MB flash are probed; two full original-flash reads match. The manufacturer demo was reported working. An initial read-only TrailGuide portrait application, v2-0.1.5 DRAFT, now runs on C6 with eight pairs, Ask/Try, Guide me and ordered Another. Actual-LVGL host tests and synthetic on-device navigation passed. Human TrailGuide screen/touch review, exact PCB identity and sustained memory/performance remain open. See [V2-TRIAL.md](V2-TRIAL.md). Filtering and tracking remain unimplemented. Authenticated V2 Wi-Fi/OTA, wrong-password rejection, partial-transfer recovery and two network updates are verified under [V2-WIFI-UPDATES.md](V2-WIFI-UPDATES.md); this trial is not Phase 2/3 release acceptance.

- Identify exact C6 board/revision, display/touch drivers, pins, memory, partitions and storage budget. Retain the original firmware if readable before an authorized write; keep V1 excluded from C6 commands.
- Run the correct C6 manufacturer display/touch examples before integration; verify any optional sensors independently. Review a separate 172x320 portrait layout/preview rather than scaling the existing circle.
- Pin actual working toolchain/dependencies; retain compatible authenticated OTA and USB recovery.
- Build independent V2 target with shared tested content/deck logic, readable card view and tap-to-draw.
- Keep touch-first controls and idle/wake behavior simple. Defer animation/shake until the relevant hardware and input safeguards are verified; suppress accidental queued actions.

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

## Parallel coaching research and practice track — P-19

Status: product direction endorsed; evidence synthesis, practice companion, pilot and additional interfaces are pending. See [COACHING-DIRECTION.md](COACHING-DIRECTION.md) and the [current handoff](HANDOFF.md).

- Investigate invitation, listening, feedback, useful effort, accessible participation and appropriate AI reliance. Distinguish study design, domain/transfer limits, uncertainty and contradictions; do not present scheduled research as completed evidence.
- Draft a small before/during/after facilitator-practice companion with synthetic scenarios. Start outside the live device and review the existing eight pairs before expanding menus or the catalog.
- Co-design and pilot with consent, passing and non-AI/access alternatives. Evaluate agreed observable usefulness and voluntary feedback, not inferred learner ability or AI-use counts.
- Select the smallest next content/UI increment after evidence and human review. New practice modes or automated role-play require a separate scope decision.
- Keep this track independent of firmware changes while preserving all existing privacy, content, storage and release gates. No hidden recording, profiling or learning-outcome claims.

## Future V3 — round ESP32-S3 port

The planned Waveshare ESP32-S3-Touch-LCD-1.85 is V3. Preserve V3-SPEC.md, ROUND-UI-SPEC.md and the existing circular browser preview. After V2 establishes the shared content/behavior layer, inspect the exact S3 board and port only the target-specific display/input/layout/storage boundaries. V2 results do not verify V3 hardware, partitions, memory or OTA.

## Working agreement

Software contributors handle code/docs/tests; hardware operators handle physical wiring and report visible/hands-on results. Update PRODUCT-SPEC.md as requirements evolve, PLAN.md as scope changes and BRINGUP.md with real evidence. Publish verified commits to the configured project origin; never publish credentials, credential-bearing binaries, private logs or real Team Trail data.

## Current implementation summary

V1 has twelve cards, address detection, debounce, shuffled draws, serial fallback and authenticated OTA. Host tests and ESP32 compilation passed; v1-0.2.4 was USB-flashed and serial-verified after correcting the OLED driver to SH1106. User confirmed display/button behavior. Prior firmware verified wireless updates, password rejection, reconnect and interrupted-transfer recovery; OTA code is unchanged. Remaining acceptance is tracked in BRINGUP.md.

The expanded forty-card catalog and eight paired draft faces are authored and content-tested. An initial read-only circular browser preview exercises modes/themes and synthetic team context for V3/shared design. V2 also has its first read-only portrait hardware trial; all eight pairs remain draft, review order is fixed and physical acceptance is pending. Production V2 behavior, real team storage and history are not implemented. The product specification defines intended runtime behavior, not a shipped feature set.

## Immediate two-mode handoff

Use HANDOFF.md for the current checkpoint rather than repeating completed bootstrap work. Preserve the running v2-0.1.5 eight-pair portrait trial, authenticated OTA and V1 fallback. Finish human guide/readability/input review and the P-19 evidence/practice brief; then choose a bounded next increment. Complete paired-content review and production eligibility/pools/events/migration before enabling recording. Retain the older round preview for V3 and align it separately; browser/synthetic results do not establish new physical acceptance. FACILITATOR-MODES.md and V2-SPEC.md/P-18 remain the runtime contracts.
