# TrailGuide — Living product specification

Status: approved product direction; implementation pending unless explicitly marked verified below.

## 1. Purpose and source of truth

TrailGuide is an offline pocket facilitator for helping workshop teams work with AI. It offers a useful next move, remembers which moves a facilitator tried with each team, and supports follow-through without distracting from the people.

This is the canonical product specification. Keep it in GitHub and update it as development proceeds. [PLAN.md](PLAN.md) defines sequencing; [V1-SPEC.md](V1-SPEC.md) and [V2-SPEC.md](V2-SPEC.md) define hardware-specific requirements; [WIFI-UPDATES.md](WIFI-UPDATES.md) defines firmware updates. Actual execution evidence belongs in [BRINGUP.md](BRINGUP.md).

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

V2: planned round touchscreen target. Exact revision, touch/display/sensor dependencies, memory layout and usable storage must be verified on the actual hardware. There is no V2 firmware or simulator yet. A mockup is not hardware verification.

## 4. Experience and navigation

### P-01: Home and quick start — core release

Home offers three large actions: **Quick bearing**, **Help my team**, **Team Trail**. A small team indicator shows the active team or **No team**. Team setup is never required to draw advice.

Quick bearing draws from the whole deck. Help my team opens theme selection; situation shortcuts are an enhancement. Back returns one level without marking, saving or drawing. Home remains reachable without a deep menu.

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

The core card view shows title, theme and concise action. Actions: **Used it**, **Save**, **Another idea**. Back and Home must remain accessible. The exact arrangement requires a round-screen prototype; do not squeeze all controls into an unreadable screen.

Optional **Show me how** opens a detail view with a spoken facilitation question, a small team activity, an example AI prompt where relevant, and an indicative time box. Build the detail view after the core path works; no live AI generates this content.

Drawing, scrolling, Back, waking and opening details never create a use event. Shake, if enabled, means draw only in an appropriate draw/card context, never while editing, confirming use, viewing history or navigating. Animation must not queue additional draws.

### P-04: Team context — core release

Default to No team. Offer a small local roster using non-identifying labels such as Team 1; do not require touchscreen typing. Proposed first limit: eight teams with labels up to 24 characters. Renaming is secondary and may initially use a local provisioning tool.

Changing team updates a clearly visible indicator. An open card may remain visible, but every Used it confirmation names the destination team. Never silently attribute a card to the previously active team.

Without an active team, Used it opens a team picker with Cancel; cancellation records nothing. Quick bearing continues to work without a team.

### P-05: Used it and feedback — core release

Used it asks **Record for Team N?**. Confirm durably appends one use event; double taps cannot duplicate it. Show **Recorded** only after storage succeeds. A failed write shows an error and allows retry without double counting.

Offer Undo for the latest action while on that card, and allow undo from recent history. Undo creates a reversal linked to the original event rather than losing the audit relationship. A reversal does not count as use.

After recording, optional **Helpful / Not yet / Skip** feedback attaches to that event. Skip is the default absence of feedback, not a negative rating. Feedback is subjective facilitator input, not a team-performance measure. Reusing the card on another occasion is allowed through an explicit Use again action/confirmation; an incidental second tap must not record another use.

### P-06: Saved cards — core release

Save is a toggle, scoped to the active team or a separate personal list when No team is active. Saving never marks Used. Offer a saved list and an explicit unsave action. Changing teams does not move saved items between teams.

### P-07: Team Trail — core release

Select a team to see:

- Recent use events, newest first, with card title, theme and optional feedback.
- Saved ideas, separately labeled from used cards.
- Themes explored, counted from non-reversed use events, not draws.
- Last tried card as a follow-through prompt: **What happened when you tried this?**

Opening an entry shows the recorded card revision's content. Repeated uses remain separate events. Show empty states honestly: **No cards recorded yet**, not fabricated progress. No automatic reminders, rankings or unrequested cross-team comparisons.

### P-08: Follow-through and favorites — next enhancement

On a team's return, offer a dismissible Last tried prompt. Optional follow-up outcomes: **Try again / Adapt it / Move on**. Never make a past outcome mandatory before drawing a new idea. Personal favorites are separate from team saves and remain easy to reach.

### P-09: Situation shortcuts — next enhancement

Initial situations: Too many ideas; Nobody's talking; AI sounds convincing; We're out of time. Each maps to a reviewed set of card IDs/tags across themes. Show the situation filter, avoid immediate repeats and allow clearing it. Empty results explain the situation and offer all themes; do not silently substitute unrelated advice.

## 5. Content specification

### P-10: Structured, reviewed content

Proposed initial V2 target: forty cards, five per theme. Preserve the twelve V1 meanings and assign stable IDs before expanding. Forty is a content target, not the number currently implemented.

Card fields:

- Stable card_id, content revision, primary theme and optional situation tags.
- Short title, concise action and optional detail fields.
- Optional activity duration and editorial/review state.
- Short OLED-compatible copy only where a card is intended for V1.

No user/team data belongs in the published content deck. IDs never change because titles are edited; retired IDs are not reused. Existing history retains enough versioned content to display what was used. Validate duplicate IDs, unknown themes, missing fields and empty pools automatically.

Review for practical action, clarity, respectful participation and safe verification. Avoid presenting AI output as verified fact or encouraging entry of confidential team material into AI services. Avoid filler variants included just to reach forty.

### P-11: Draw policy

Shuffle without repeats within each active pool cycle and avoid an immediate repeat when reshuffling. Whole-deck, theme and situation pools have distinct cycle state. Used and saved cards are not automatically excluded: an explicitly reused exercise may still be useful. Future Untried only is optional and must handle exhausted pools explicitly.

Pool state may restart after reboot; durable use history must not. A one-card pool may repeat and must not hang attempting to avoid the unavoidable repeat. V1 retains its existing whole-deck behavior until a separately tested change is accepted.

## 6. Local data, integrity and privacy

### P-12: Logical data model

- Team: stable team_id, non-identifying label, created order and archive state.
- Card: stable card_id, revision and versioned content or a retained snapshot.
- Use event: event_id, team_id, card_id/revision or snapshot reference, event sequence, session/boot ID, optional trusted wall time and optional feedback.
- Reversal: event_id and referenced use event.
- Saved reference: team_id or personal scope, card_id and saved order.
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

Keep text and controls inside the actual usable circle and away from clipped edges. Prefer large tap targets, short pages, high contrast and text labels instead of color-only meanings. Validate on real hardware in workshop lighting; numeric sizing must follow actual display dimensions and touch calibration.

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
- P-15: every screen/card fits and is readable on the actual circle; scroll/wake/shake cannot cause accidental recording; essential paths work by touch.
- Real hardware: manufacturer examples, one-hour USB soak, reboot and retained trail, plus OTA retention/recovery checks on the V2 implementation. Battery readiness requires separate measured tests.

Host tests are required for deck, event attribution, deduplication, reversal, counts, persistence failure handling and migration logic. A browser/mock UI review can validate workflow but cannot prove ESP32 performance, touch accuracy or flash durability.

## 9. Delivery scope and fallback

Core release: P-01 through P-07, P-10 through P-15; P-03 optional detail expansion can follow. A basic persistence layer is inseparable from releasing Used it or Team Trail: do not ship misleading volatile history.

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

### Open validation decisions

- Confirm team roster size and whether preset labels suffice during workshop use.
- Review a circular-screen prototype for card actions, confirmations and theme navigation.
- Review forty-card editorial set before publishing or loading it.
- Measure storage budget and choose persistence/backup implementation on the actual V2 board.
- Confirm which enhancements fit before the release scope freeze; core reliability wins.
