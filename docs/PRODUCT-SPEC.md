# TrailGuide — Living product specification

Status: approved product direction; implementation pending unless explicitly marked verified below.

## 1. Purpose and source of truth

TrailGuide is an offline pocket facilitator for helping workshop teams work with AI. It helps the facilitator ask useful questions, listen and choose an optional activity through Ask the Team and Try a Move modes. It offers a useful next move, remembers which moves a facilitator tried with each team, and supports follow-through without distracting from the people.

This is the canonical product specification. Keep it in GitHub and update it as development proceeds. [PLAN.md](PLAN.md) defines sequencing; [V1-SPEC.md](V1-SPEC.md), [V2-SPEC.md](V2-SPEC.md) and [V3-SPEC.md](V3-SPEC.md) define hardware-specific requirements; [WIFI-UPDATES.md](WIFI-UPDATES.md) defines firmware updates. Actual execution evidence belongs in [BRINGUP.md](BRINGUP.md).

The product direction includes Team Trail, theme selection, a simple start mode and supporting facilitation ideas. Numeric limits, exact labels and phase boundaries below are proposed engineering defaults, not individually user-approved decisions. Validate them during prototype review.

## 2. Principles

- Useful immediately: no mandatory team setup, connectivity or tutorial.
- Start simple, reveal depth: one useful card first, optional guidance second.
- Facilitate people, not just prompts: support inclusion, decisions and experiments.
- Record deliberate use, not screen views. Seeing or saving a card is not using it.
- Remember teams, not participant identities. No leaderboards or performance scoring.
- Work offline, including history. Pi and internet are not runtime requirements.
- Preserve V1 as a dependable fallback; do not let V2 scope destabilize it.
- Never infer that a team improved merely because a card was used or rated helpful.

## 3. Hardware and current reality

V1: ESP32 + SH1106 OLED, BOOT input, firmware v1-0.2.4 with twelve cards in a single shuffled deck. It has no implemented themes, team storage, saved cards or history UI. The user confirmed display/button behavior; full acceptance remains open in BRINGUP.md.

V2: current ESP32-C6 1.47-inch 172x320 portrait capacitive-touch target with pin headers. C6 silicon revision 0.2 and 8MB flash are probed; the original image is preserved in two matching full-flash reads. The manufacturer LVGL display/touch demo is compiled, USB-flashed and serial-verified, not a TrailGuide application. Visible pixels/touch mapping and physical PCB identity still need human checks; runtime memory/performance remains unmeasured. See [V2-BRINGUP.md](V2-BRINGUP.md). V2 needs a separate portrait layout, not a scaled round UI.

V3: future round Waveshare ESP32-S3-Touch-LCD-1.85 target, nominal 360x360. The existing eight-card circular browser preview is retained for V3 and shared workflow review; it is not V2 portrait acceptance, board emulation, hardware verification or durable tracking. No production V2/V3 firmware exists.

## 4. Experience and navigation

### P-01: Home and quick start — core release

Home offers two primary actions: **Ask the Team** and **Try a Move**, with **Team Trail** secondary and a visible active-team/No team indicator. Ask is the visually prioritized fresh-session choice. Fresh opening/reboot lands on Home without an automatic draw. A deliberate mode tap starts an unfiltered draw immediately; no team or theme setup is required.

Quick bearing remains the behavior of an unfiltered draw, rather than an extra home step. Themes are optional filters in either mode. Back returns one level without marking, saving or drawing. P-16 and [FACILITATOR-MODES.md](FACILITATOR-MODES.md) define the two-mode flow.

### P-02: Themes — core release

Eight themes organize the content. Each card has one primary theme; optional tags support future cross-theme situations.

- Explore — We need possibilities.
- Clarify — We're not sure what we're solving.
- Experiment — We need something small to try.
- Verify — Can we trust this?
- Reflect — What did we learn?
- Unstick — We're going in circles.
- Include — We need more voices involved.
- Decide — We need to choose and move.

Show a readable scrolling list, not eight tiny targets around the circle. Selecting a theme draws from that theme. The active filter remains visible and is easy to clear.

### P-03: Card and optional depth — core release / enhancement

The main card prioritizes theme, mode and either a spoken question (Ask) or concise action (Try). The full title/reference is accessible in Guide me and event/confirmation detail rather than crowding the question. **Guide me** and **Another** are the two bottom actions; header Back, team and More remain visible. More holds **Asked this** or **Offered this move**, **Save/Saved** and related-face navigation. **Guide me** is core facilitator support in both modes, with Say, Notice and optional Follow up; it is not a third mode. P-17 and ROUND-UI-SPEC.md define the proposed circular layout and discoverable navigation.

There is one help entry: **Guide me**. After core Say/Notice/Follow-up works, optional Steps and AI prompt pages add depth inside that view; do not create a competing Show me how button. No live AI generates this content. For cards in the Agent Workflow collection, optionally reveal a tool-neutral agent task and human check, with separate Codex and Claude Code notes. Tool notes are curated guidance, not executable integrations. Collection selection and tool-note panels are enhancements, not implemented UI.

Drawing, scrolling, Back, waking and opening details never create a use event. Shake, if enabled, means draw only in an appropriate draw/card context, never while editing, confirming use, viewing history or navigating. Animation must not queue additional draws.

### P-04: Team context — core release

Default to No team. Offer a small local roster using non-identifying labels such as Team 1; do not require touchscreen typing. Proposed first limit: eight teams with labels up to 24 characters. Renaming is secondary and may initially use a local provisioning tool.

Changing team updates a clearly visible indicator. An open card may remain visible, but every record-use confirmation names the destination team and mode. Never silently attribute a card to the previously active team.

Without an active team, Asked this / Offered this move opens a team picker with Cancel; cancellation records nothing. Quick bearing continues to work without a team.

### P-05: Recording use and feedback — core release

The underlying record-use action is labeled **Asked this** in Ask and **Offered this move** in Try, reached through More. Its confirmation names the full team identity and mode: **Record Asked for Team N?** or **Record Offered move for Team N?**. Confirm durably appends one use event; double taps cannot duplicate it. Show **Recorded** only after storage succeeds. A failed write shows an error and allows retry without double counting.

Offer Undo for the latest action while on that card, and allow undo from recent history. Undo creates a reversal linked to the original event rather than losing the audit relationship. A reversal does not count as use.

After recording, optional **Helpful / Not yet / Skip** feedback attaches to that event. Skip is the default absence of feedback, not a negative rating. Feedback is subjective facilitator input, not a team-performance measure. Reusing the card on another occasion is allowed through an explicit Use again action/confirmation; an incidental second tap must not record another use.

### P-06: Saved cards — core release

Save is scoped to the active team or a separate personal list when No team is active. An already-saved card shows Saved; Unsave is a separate explicit action, not a side effect of reopening Saved. Saving never marks Used. Offer a saved list and an explicit unsave action. Changing teams does not move saved items between teams.

### P-07: Team Trail — core release

Select a team to see:

- Recent use events, newest first, with card title, theme, Asked/Tried mode label and optional feedback. Legacy events without mode are labeled unspecified, never guessed.
- Saved ideas, separately labeled from used cards.
- Themes explored, counted from non-reversed use events, not draws.
- Last tried card as a follow-through prompt: **What happened when you tried this?**

Opening an entry shows the recorded card revision's content. Repeated uses remain separate events. Show empty states honestly: **No cards recorded yet**, not fabricated progress. No automatic reminders, rankings or unrequested cross-team comparisons.

### P-08: Follow-through and favorites — next enhancement

On a team's return, offer a dismissible Last tried prompt. Optional follow-up outcomes: **Try again / Adapt it / Move on**. Never make a past outcome mandatory before drawing a new idea. Personal favorites are separate from team saves and remain easy to reach.

### P-09: Situation shortcuts — next enhancement

Initial situations: Too many ideas; Nobody's talking; AI sounds convincing; We're out of time. Each maps to a reviewed set of card IDs/tags across themes. Show the situation filter, avoid immediate repeats and allow clearing it. Empty results explain the situation and offer all themes; do not silently substitute unrelated advice.

### P-16: Two-mode facilitator coaching — core release

Approved direction; implementation pending. Detailed content, UI, storage and handoff requirements: [FACILITATOR-MODES.md](FACILITATOR-MODES.md).

- **Ask the Team** gives a natural open question. **Try a Move** gives a practical activity. Ask is the default for a fresh session.
- **Guide me** coaches the facilitator: Say, observable Notice cues and optional Follow up. It is available in both modes and is core, not deferred Show me how content.
- Reuse forty stable card identities and eight themes; author reviewed paired faces rather than duplicate cards or create mode-specific themes. Preserve V1 and existing agent variants.
- Mode is independent of team, theme and optional Agent Workflow collection. FACILITATOR-MODES.md defines the proposed draft/reviewed/retired state scopes and separate Ask/Try Guide me eligibility. Apply one eligibility decision across draws, linked faces, saves and explicit reuse; empty pools explain the absence without silently substituting advice. Historical snapshots remain clearly labeled read-only history.
- Related-face navigation preserves the current card/revision. Another draws within current mode/filter. Shake draws only from main card screens; Guide me and other navigation/confirmation states suppress it. One Guide me entry contains core support and later depth; record labels are Asked this / Offered this move.
- Asking, reading, waiting and switching faces record nothing. Deliberate confirmed use records the actual mode and destination team. Ask records use of a question; Try records offering/starting an activity, not completion or learning.
- Save remains per scope/card and retains originating mode. History migration preserves old records as unspecified. Do not enable mode-aware history before durability/migration checks pass.
- No answer capture, voice listening, automated learner diagnosis or mandatory transition into advice.

### P-17: V3 round-screen simplicity and prototype gate

An initial read-only browser preview is built for eight representative draft pairs. Production firmware/tracking and facilitator/actual-hardware acceptance remain pending. Detailed coordinates, task paths, overflow handling and acceptance: [ROUND-UI-SPEC.md](ROUND-UI-SPEC.md).

- Prioritize the question/action. Main card bottom actions are Guide me and Another; Back, team and More stay visible. Keep full title/reference in guidance/details and secondary operations in a paged action list.
- Guide me is the only help entry; later Steps/AI/tool notes extend it, not a second Show me how button. Record-use labels are Asked this / Offered this move, with a separate named-team/mode confirmation.
- Start from a 360x360 canvas and a proposed 12-pixel radial inset. Validate full rectangles, text, focus rings and scroll cues against the circle. Proposed fonts, 48-pixel minimum target heights and 52-pixel primary controls need actual rendering/touch review; geometry alone is not acceptance.
- Use measured text layout, readable type, explicit paging/overflow and labeled tap paths. Do not truncate essential copy, shrink text to fit or rely on hidden gestures. Wake/scroll/navigation cannot record or draw unexpectedly.
- First author eight representative draft pairs, one per theme, and a read-only round prototype with an overflow fixture. Recording/saving remain visibly disabled; use synthetic team context and no fake persisted history.
- Review unprompted facilitator task completion and revise before completing forty paired faces or implementing storage migration. Eligibility, pools, durability and actual V3 display/touch/OTA gates follow; preserve V1 throughout.

### P-18: V2 portrait ESP32-C6 adaptation — current target

The current V2 is the 1.47-inch ESP32-C6 capacitive-touch board with pin headers and nominal 172x320 portrait screen. V3 is the future round ESP32-S3 board. Hardware and bring-up contract: [V2-SPEC.md](V2-SPEC.md); preserve the separate [V3 contract](V3-SPEC.md).

- Re-identify the physical board/revision, chip, flash/partitions, drivers and serial port before selecting examples or writing firmware. USB enumeration is not chip/display/touch verification.
- Preserve V1 as fallback and retain original C6 firmware if readable before an authorized write. No V1 binary, wiring map or V3 display configuration is assumed compatible.
- Reuse stable cards, themes and P-16 behavior; make a separate portrait layout with readable text and stacked labeled controls. Do not shrink/crop the circular preview to fit 172x320.
- Establish correct manufacturer display/touch examples, orientation, touch mapping and actual memory budget before TrailGuide integration. Validate a read-only eight-card portrait preview and actual-device readability before enabling tracking.
- Authenticate updates and independently verify partitions, data retention, reboot, soak and recovery on V2. Prior V1 tests and browser results do not prove C6 behavior.
- Keep motion/animation optional and gated by separately verified hardware. Preserve the existing round preview and P-17 as V3 work, not completed V2 acceptance.

## 5. Content specification

### P-10: Structured, reviewed content

The [card catalog](CARD-CATALOG.md) now contains forty draft cards, five per theme, backed by [structured content](../content/cards.json). The twelve V1 titles and action meanings are preserved verbatim with stable IDs tg-001 through tg-012; twenty-eight new cards use tg-013 through tg-040. Each existing entry has revision, theme, action, `question`, activity, optional AI prompt, suggested minutes, situation tags and draft review state. Eight representative cards, one per theme, now have additive draft Ask/Notice/Follow-up and Try invitation/Notice/Follow-up fields. Their revisions advanced and the readable catalog is synchronized. The other thirty-two cards do not yet have paired guidance. None is editorially reviewed or integrated into production firmware. FACILITATOR-MODES.md defines exact draft/reviewed/retired scopes and all-entry-point eligibility; no existing `question` or draft action is automatically approved by those proposals. Content-contract tests pass; facilitator editorial review, screen-layout review and runtime integration remain pending. V1 still runs its unchanged twelve-card deck.

An optional [Agent Workflow collection](AGENT-WORKFLOW.md) now adapts ten existing cards for supervising CLI coding agents. It does not create a ninth theme or increase the forty-card count. Structured collection metadata references stable card IDs; each adapted card has agent_workflow label, prompt, human check and codex/claude_code tool notes. Agent-workflow entries have revision 2 or later; paired facilitator enrichment advances the affected card revision again. Canonical JSON is authoritative for each revision. These optional additions retain schema_version 1 and leave existing base fields intact. Unsupported optional fields may be ignored by readers; required data remains validated.

Keep general advice tool-neutral. Confirm tool versions, settings and active project instructions before using product-specific guidance; do not teach permission bypass as a default. No CLI command, session or private prompt is executed or collected by TrailGuide. The collection and notes are authored drafts, not evidence of runtime support.

Card fields:

- Stable card_id, content revision, primary theme and optional situation tags.
- Short title, concise action and optional detail fields.
- Optional activity duration and editorial/review state.
- Short OLED-compatible copy only where a card is intended for V1.

No user/team data belongs in the published content deck. IDs never change because titles are edited; retired IDs are not reused. Existing history retains enough versioned content to display what was used. Validate duplicate IDs, unknown themes, missing fields and empty pools automatically.

Review for practical action, clarity, respectful participation and safe verification. Avoid presenting AI output as verified fact or encouraging entry of confidential team material into AI services. Avoid filler variants included just to reach forty.

### P-11: Draw policy

Shuffle without repeats within each active pool cycle and avoid an immediate repeat when reshuffling. Pool keys include mode and active whole-deck/theme/situation/collection filters, with distinct cycle state. Linked Ask/Try face navigation preserves card identity and does not consume a random draw; see P-16. Used and saved cards are not automatically excluded: an explicitly reused exercise may still be useful. Future Untried only is optional and must handle exhausted pools explicitly.

Pool state may restart after reboot; durable use history must not. A one-card pool may repeat and must not hang attempting to avoid the unavoidable repeat. V1 retains its existing whole-deck behavior until a separately tested change is accepted.

## 6. Local data, integrity and privacy

### P-12: Logical data model

- Team: stable team_id, non-identifying label, created order and archive state.
- Card: stable card_id, revision and versioned content or a retained snapshot.
- Use event: event_id, team_id, mode (ask/try; legacy unspecified), card_id/revision or snapshot reference, event sequence, session/boot ID, optional trusted wall time and optional feedback. A future optional variant_id can distinguish agent-workflow use from the base activity without changing card identity; migration and display behavior must be tested before introducing it. Do not store raw CLI transcripts or prompt inputs.
- Reversal: event_id and referenced use event.
- Saved reference: team_id or personal scope, card_id, originating mode and saved order. Deduplicate per scope/card, retain originating mode, and do not duplicate saves when opening the other face.
- Follow-up: use-event reference and selected outcome; deferred until P-08.
- Settings: schema version, roster, active team, display/interaction preferences.

Physical storage technology and partition sizing are implementation decisions after hardware inspection. Use stable serialized formats and a tested migration path. Runtime data stays out of GitHub and source builds.

### P-13: Persistence and honest time

Core history, saves, roster and feedback survive ordinary restart and power loss. Use atomic or recoverable commits with integrity checks and bounded writes; do not write every animation frame or touch.

No reliable calendar clock is assumed. Preserve ordered event sequence and session identity; label wall time unknown unless a trusted time source was explicitly established. Never invent timestamps or display uptime as a calendar date.

Proposed bounded acceptance: eight teams and 1,000 use events total, with room for saves and reversal metadata. Benchmark actual footprint and latency before accepting these limits. Warn before full; never silently erase old history. If full, advice remains usable and recording explains that storage is full. Metadata/undo capacity must be accounted for separately and tested.

On corruption, retain recoverable records, show the failure and keep advice available. Do not silently initialize an empty trail. Older firmware must not overwrite an unknown newer schema. Firmware update and interrupted migration tests must distinguish preserved data from deliberately unsupported downgrade paths.

### P-14: Management and privacy

No participant names, team discussion transcripts or free-text notes in the first release. Team labels can still identify a group; keep all runtime trails private and offline. Do not log team events over serial by default or upload them as diagnostics.

Team archive preserves history. Destructive delete/reset requires a named-scope confirmation and clear distinction between one team and all data. No background telemetry, cloud sync or automatic sharing.

A local export/backup and restore tool is a post-core priority, using explicit user action and a versioned format. Warn that exports may contain private team labels. No secure at-rest encryption claim is made; physical access to the device remains a privacy consideration. Before real workshop collection, explicitly decide whether lack of backup is acceptable.

## 7. Round-screen interaction and accessibility

### P-15: Readability and input

Keep text and controls inside the active display's usable area: rectangular portrait for V2, circle-safe for V3. Prefer large tap targets, short pages, high contrast and text labels instead of color-only meanings. Validate on real hardware in workshop lighting; numeric sizing must follow actual display dimensions and touch calibration.

Scrolling must not trigger buttons. Wake preserves the current card and consumes the first touch instead of marking use or drawing. Disable shake where handling could mutate history. Idle dimming does not lose team context or saved state. All essential actions work by touch; shaking is optional.

A compass needle is a visual metaphor, not a magnetic heading instrument. Do not imply real north without appropriate verified hardware.

## 8. Verification and release gates

Each requirement must be labeled Planned, Implemented, Host-tested, Prototype-reviewed or Hardware-verified with evidence. Do not infer completion from a checked task list or generated code.

Required checks for the core release:

- P-01/02/03: Quick bearing works without setup; eight themes filter correctly; back/wake/draw never mark use.
- P-04/05: switching teams cannot misattribute a use; cancel writes nothing; rapid double taps record once; confirmed reuse records separately; Undo and feedback affect the intended event only.
- P-06/07: saved and used are separate, team trails are isolated, reversals update counts, empty states and theme coverage are correct.
- P-10/11: all published cards validate; stable IDs/revisions survive edits; every pool cycles correctly including one/zero-card cases.
- P-12/13/14: reboot, interrupted writes, full storage, recoverable corruption, schema migration and unknown-schema behavior are tested with synthetic data. No real team data enters Git or diagnostics.
- P-16: default Ask, immediate mode entry, Guide me, linked-face navigation, mode-aware pools/eligibility/saves/events and legacy migration pass FACILITATOR-MODES.md checks. No question/activity is automatically recorded.
- P-15: every screen/card fits and is readable on the active target geometry (V2 portrait or V3 round); scroll/wake/shake cannot cause accidental recording; essential paths work by touch.
- Real hardware: manufacturer examples, one-hour USB soak, reboot and retained trail, plus OTA retention/recovery checks on the V2 implementation. Battery readiness requires separate measured tests.

Host tests are required for deck, event attribution, deduplication, reversal, counts, persistence failure handling and migration logic. A browser/mock UI review can validate workflow but cannot prove ESP32 performance, touch accuracy or flash durability.

## 9. Delivery scope and fallback

Shared core release: P-01 through P-07 and P-10 through P-16. Apply P-18 for current V2 portrait and P-17 for future V3 round; do not require V3 hardware acceptance to release V2. P-03 optional detail expansion can follow. A basic persistence layer is inseparable from releasing Used it or Team Trail: do not ship misleading volatile history.

Next enhancements: P-08 follow-through/favorites, P-09 situations, expanded card details, explicit local export/restore. Subsequent possibilities require a separate scope decision: timers, richer notes, phone companion, cloud sync, live AI, audio or analytics.

Freeze release scope after core acceptance; reserve a stabilization phase for fixes, power checks, recovery and facilitator rehearsal. Releases are gated by evidence, not a specific workshop date. If core V2 fails acceptance, use tested V1; a stable basic V2 may be retained only with missing Team Trail features clearly disclosed. Do not present incomplete history as working.

## 10. Living-spec maintenance

For each meaningful change:

1. Update the affected P-ID and acceptance criteria before or alongside implementation.
2. Add a decision below describing why scope/behavior changed; use Git history for chronology rather than embedding calendar dates.
3. Update PLAN.md status and BRINGUP.md execution evidence; record known blockers.
4. Keep planned behavior separate from shipped behavior and preserve requirement IDs.
5. Commit related code/tests/docs together; publish and verify GitHub readback. Never include credentials, private builds or real team trails.

### Decision log

- Expand from a shuffled advice oracle toward a pocket facilitator with themes, simple start, Team Trail and supporting facilitation ideas. Added eight proposed themes, team-scoped deliberate-use tracking, saved ideas and phased follow-through. Hardware work remains gated on board availability and inspection.

- Content expansion: forty draft cards, five per theme, with stable IDs/revisions and optional guidance. Keep JSON as the canonical editorial source and the Markdown catalog synchronized. Preserve V1's twelve titles/actions without a firmware change; facilitator review and hardware layout acceptance remain separate gates.

- Agent workflow: adapt ten existing cards for CLI-agent supervision, keeping forty cards and eight themes. Add a cross-theme collection, optional tool notes and human evidence checks rather than duplicating cards or replacing general facilitation guidance. Collection UI remains deferred until the core path works.

- Facilitator coaching: introduce Ask the Team and Try a Move with default Ask and core Guide me support. Supersede the earlier three-action home proposal. Preserve card identities, V1 firmware and optional agent variants; require mode-aware tracking and migration before releasing two-mode history. Detailed implementation handoff is FACILITATOR-MODES.md.

- Round UI planning: use a single Guide me entry, mode-specific recording labels and progressively disclosed controls. Correct the existing `question` field reference, define proposed review/eligibility scopes across all new-use paths, and gate full content/runtime expansion on an eight-card read-only round prototype. Numeric layout and interaction parameters remain proposed until reviewed and measured.

- Hardware sequence: the newly connected portrait ESP32-C6 is V2 and the future round ESP32-S3 is V3. Preserve V1 as fallback, retain the round preview/spec as V3 work, and require a separately reviewed portrait adaptation plus actual C6 bring-up for V2. No device was reset or flashed by the designation change.

### Open validation decisions

- Confirm team roster size and whether preset labels suffice during workshop use.
- Review a circular-screen prototype for card actions, confirmations and theme navigation.
- Review forty-card editorial set before publishing or loading it.
- Measure storage budget and choose persistence/backup implementation on the actual V2 board.
- Confirm which enhancements fit before the release scope freeze; core reliability wins.
