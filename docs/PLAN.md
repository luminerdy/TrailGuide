# Delivery plan

Goal: dependable offline facilitation support for AI workshops using the reusable Frontier Ops theme. Preserve V1 as the fallback.

Canonical requirements: [Living product spec](PRODUCT-SPEC.md). Hardware contracts: [V1](V1-SPEC.md), [V2](V2-SPEC.md). Execution evidence: [BRINGUP.md](BRINGUP.md). Planned work is not implemented functionality.

## Phase 0 — Finish and preserve V1

Status: in progress. v1-0.2.4 SH1106 display and mechanical BOOT behavior are user-confirmed; full acceptance is not complete.

- Finish fifty-press/thirty-minute, full-deck readability, restart and independent-power checks.
- Record human observations separately from serial/host-test results.
- Preserve known working firmware, credentials and USB recovery instructions locally; publish source/docs only.
- Keep V1's twelve-card whole deck stable while V2 develops.

Exit: V1 acceptance evidence recorded, recovery usable and all remaining limitations stated.

## Phase 1 — Content and workflow design, before V2 integration

Status: product spec and forty-card draft catalog authored; content-contract tests pass. Facilitator review, circular-screen prototype and runtime integration remain pending.

- Stable IDs/revisions are assigned to all forty draft cards, preserving the twelve existing titles/actions without changing V1 firmware.
- Eight themes have five cards each in [CARD-CATALOG.md](CARD-CATALOG.md), sourced from [content/cards.json](../content/cards.json). Ten existing cards also have draft agent-workflow variants with Codex/Claude Code notes and human checks, indexed in [AGENT-WORKFLOW.md](AGENT-WORKFLOW.md). No new theme or cards are added. Complete facilitator editorial review before inclusion.
- Prototype Home, Quick bearing, themes, card actions, team picker and Team Trail on a circular 360x360 canvas.
- Review No team, saved-vs-used, team switch, confirmation/Undo, empty trail and write-failure states.
- Define synthetic fixtures and tests for content/deck/use-event/storage logic.

Exit: facilitator review of navigation and representative cards is complete; stable data contracts and acceptance tests are defined. Prototype approval does not establish hardware readiness.

## Phase 2 — V2 hardware bring-up, when hardware is available

Status: pending actual-board inspection and bring-up.

- Identify exact board/revision, touch/display/IMU drivers, pins, memory, partition layout and storage budget.
- Run manufacturer display, touch and sensor examples before application integration.
- Pin actual working toolchain/dependencies; retain compatible authenticated OTA and USB recovery.
- Build independent V2 target with shared tested content/deck logic, readable card view and tap-to-draw.
- Add nonblocking animation, optional shake, idle/wake; suppress accidental queued actions.

Exit: core touch/display path works on actual hardware with evidence. No team-history release until durable storage passes Phase 3.

## Phase 3 — Core release: themes and Team Trail

Status: planned. Requirements P-01–P-07 and P-10–P-15 in PRODUCT-SPEC.md.

- Quick bearing without setup; eight-theme selection.
- Small preset team roster and persistent active-team indicator.
- Used it confirmation, deduplicated durable event, Undo and optional feedback.
- Team-scoped saved ideas; personal saves when No team is selected.
- Team Trail: recent uses, separate saves, themes explored and last tried prompt.
- Recoverable bounded storage, honest event ordering without an assumed real-time clock, schema/version safeguards and explicit full/error states.
- Validate privacy, accidental input prevention, attribution, reboot, power-interrupted writes, migration and corruption behavior with synthetic data.

Exit: all applicable product-spec acceptance checks pass; actual-device screen review, retained-trail reboot, one-hour USB soak and V2 OTA/data-retention checks recorded. No real team data in GitHub.

## Phase 4 — Depth and follow-through, only after core

Status: planned, dependent on core acceptance.

Priority order:
1. Show me how: spoken question, activity and optional example AI prompt.
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

## Working agreement

Software contributors handle code/docs/tests; hardware operators handle physical wiring and report visible/hands-on results. Update PRODUCT-SPEC.md as requirements evolve, PLAN.md as scope changes and BRINGUP.md with real evidence. Publish verified commits to the configured project origin; never publish credentials, credential-bearing binaries, private logs or real Team Trail data.

## Current implementation summary

V1 has twelve cards, address detection, debounce, shuffled draws, serial fallback and authenticated OTA. Host tests and ESP32 compilation passed; v1-0.2.4 was USB-flashed and serial-verified after correcting the OLED driver to SH1106. User confirmed display/button behavior. Prior firmware verified wireless updates, password rejection, reconnect and interrupted-transfer recovery; OTA code is unchanged. Remaining acceptance is tracked in BRINGUP.md.

The expanded forty-card draft catalog and its structured content are authored and content-tested. V2 firmware, circular-screen prototype, runtime theme filtering, teams and history are not implemented. The product specification defines intended runtime behavior, not a shipped feature set.
