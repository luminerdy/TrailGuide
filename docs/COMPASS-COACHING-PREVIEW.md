# Compass coaching — read-only preview

## Native follow-on — v3-0.1.14

Following approval of the browser flow and compact icons/short labels, the separately previewed compact native version is installed and exact-version startup/IMU verified. **Tap the classic compass → Coaching**; use coaching **Options → Classic** to return. Native-renderer ASan/UBSan checks and 80 isolated Python checks pass, with application-only write/explicit flash verification. New physical coaching readability/touch and natural-opener gesture acceptance remain pending. The browser file remains a separate read-only study, not a hardware emulator or physical acceptance record. Native question/experiment/closing selection advances in authored order; opener draws shuffle. The classic boot default, canonical catalog, sensor/gesture math and original reveal/hold renderer remain preserved. The compact device variant uses 18px body/12px perimeter and opening Options text, outward icons and a 192x112 blue face, trimmed 8px on each side with unchanged text wrapping. Options has no visible button box but retains its 100x48 target. The browser artifact remains the earlier 20px/14px icon study; native and browser are not claimed pixel-identical.

## Scope and status

Approved scope: a small browser interaction/content preview before any firmware integration. This is an optional facilitator aid, not an automated coach, a validated coaching intervention or a replacement for the current installed V3 experience. Browser tests and screenshots are not physical display, touch or gesture acceptance.

The existing V3 turn-over-and-back, blackout, six-second whole-face reveal and five-second reading hold remain unchanged on the device. V1/V2 behavior, the forty-card catalog, review states and existing generated consumers are not changed by this preview. No device update is authorized merely by opening this study.

Primary surface: **Explore**. The design supports finding a possible question and returning attention to people, rather than monitoring a team or completing a workflow. Frontier Ops remains a reusable visual theme. The learner-facing loop stays **Define → Direct → Check → Adjust**; the paths and listening reminders are private facilitator aids, not a competing curriculum.

## Open the study

Open [`prototype/compass-coaching.html`](../prototype/compass-coaching.html) directly in a browser. The generated HTML is portable, contains its own content/styles/scripts, and needs no server, account or network. Download/open it as an HTML document rather than expecting an attachment viewer to execute scripts.

Content and navigation are exploratory drafts. There is no recording, saving, history, analytics, live AI, participant profiling or persistent session. In-memory navigation context is not a team record and disappears when the page reloads.

External review controls are outside the modeled circular display. Turnover is **simulated**; the study does not establish physical sensing or replace the accepted native animation reference. Ordinary coaching navigation is immediate. No shake or new gesture combination is proposed.

## Content architecture

- [`content/coaching.json`](../content/coaching.json) is the editable coaching layer: stable IDs, entry types, path membership, draft status, short wording or an explicit source-text reference, optional facilitator guidance and related cards.
- [`content/cards.json`](../content/cards.json) remains the canonical forty-card catalog. Exact reused wording should resolve from an allowed field rather than be copied into another editable deck.
- A related-card link indicates a useful conceptual connection, not necessarily identical wording or an automatic instruction to perform the card's activity.
- Legacy `question`, paired `facilitator.ask` and practical `activity` have different purposes. Referencing one does not approve it as another, promote its review state or add it to a hardware selection.
- [`tools/build_compass_coaching.py`](../tools/build_compass_coaching.py) validates and resolves the layer into a deterministic self-contained preview from [`prototype/compass_coaching_template.html`](../prototype/compass_coaching_template.html). Content edits do not require changing navigation logic.

Reuse candidates include:

- **Progress:** SURPRISE ON THE TRAIL (`tg-025`), KEEP OR ADAPT (`tg-026`), MAKE CAMP (`tg-011`).
- **Stuck:** NAME THE BLOCKER (`tg-028`), SHRINK THE CLIMB (`tg-029`), CHANGE THE MEDIUM (`tg-030`).
- **Unclear:** FOG AHEAD (`tg-002`), NAME THE DESTINATION (`tg-016`), UNPACK THE WORD (`tg-017`).
- **Exploring:** SCOUT (`tg-001`), CHANGE COURSE (`tg-012`), TWO TRAILS (`tg-037`).
- **AI Results:** SHOW THE TARGET (`tg-010`), MAP THE GAPS (`tg-009`), ONE VARIABLE (`tg-018`), CHECK BEARINGS (`tg-003`).
- **Team:** QUIET FIRST (`tg-031`), MAKE ROOM (`tg-032`), MISSING VOICE (`tg-034`).
- **Next step:** NAME THE OWNER (`tg-039`), CLOSE THE LOOP (`tg-040`), REVERSIBLE STEP (`tg-038`).

These are editorial relationships, not a claim that every candidate is included or production-approved.

## Interaction contract

The six path positions are Progress at 12 o'clock, Stuck at 2, Unclear at 4, Exploring at 6, AI Results at 8 and Team at 10. They describe possible conversation contexts, never diagnoses or permanent classifications.

- Show one randomly selected opener; thereafter nothing advances without a deliberate action.
- Supply a touch route for another opener and a clearly labeled simulated-turnover review action. Motion simulation must not replace a path question, experiment, help or closing view.
- Allow direct path selection, another question, an optional experiment, path switching and an unrestricted next-step/release route.
- Remember each path's last content in RAM. Back from guidance, reminders, path selection or closing restores the prior context without drawing again. Nested returns must work too.
- Questions need not be answered or even read before switching or leaving. There are no question counts, prescribed sequences or mandatory experiments.
- Release is available even when there is no next action to confirm. A closing question is an invitation, not a required commitment or a success screen.
- Keep listening reminders private and on demand. No timed reminders or automatic interruptions.
- Avoid adjacent repeats where a pool has alternatives; handle a single item honestly rather than looping forever.
- Guard real pointer input against outside-circle presses, dragging into another target, repeat contacts and stale actions after a view changes. Keyboard operation remains available in the browser.

## Editorial guardrails

- Start with neutral curiosity. The shorter OPEN-01 is “How's the project going?”; working/challenging can be optional follow-up rather than a multi-part required opening.
- Prefer “What's working well?” to assuming exceeded expectations, and exploration before premature ranking.
- Ask about AI output fit before assuming disappointment. “What part felt least useful?” belongs only after someone describes an unhelpful output.
- Offer experiments conditionally. Do not convert open questions into disguised instructions or require a suggestion after listening.
- AI critique is a possible input, not verification. Appropriate original sources, observations and executable checks can provide evidence; AI/tool assistance may help conduct checking.
- Human-only work is complete. Appropriate requested direct help remains legitimate; independence does not mean withholding support.
- Participation may include speaking, writing, drawing, pairs, observing or passing. Do not infer motivation, understanding or agreement from silence.
- Support individuals as well as teams, and meaningful projects developed with AI assistance even if the finished artifact contains no AI.

## Validation and evidence boundaries

Automated content checks should reject duplicate/malformed IDs, unknown paths/types, invalid source references, missing content and inappropriate review states. Generation must be reproducible, escape embedded JSON safely and render text as text rather than HTML. Synthetic fixtures must not alter canonical content or overwrite an accepted preview during tests.

Browser checks should exercise all paths and optional experiments, exact return context, switching without restarting, unrestricted release, pool boundaries, long/markup-like text, real pointer and keyboard interaction, narrow viewport access and absence of script errors. Verify focus envelopes and non-overlapping touch targets inside the proposed safe circle, with readable type and a visible full-content route rather than shrinking text to pass.

Use the five proposed conversation scenarios as behavioral checks, not fabricated participant outcomes:

- **A — Progress:** explore a working prototype's discoveries without demanding a problem or corrective experiment.
- **B — AI outputs:** clarify the intended result and actual mismatch before treating prompt changes as the answer.
- **C — Unclear objective:** invite a definition of success without selecting the team's solution.
- **D — Differing opinions:** surface perspectives and optional participation without inventing agreement or deciding who is correct.
- **E — Chosen next step:** release immediately with zero additional questions or experiments.

The review question is: **Does this help the facilitator return attention to people, or add device navigation to manage?** Existing role-play observations can inform review; another prescribed rehearsal programme is not an acceptance prerequisite. Voluntary human feedback and later physical usability remain separate from executable checks.

## Verified browser checkpoint

The generated artifact contains six openers, twenty-four path questions, eighteen optional experiments, three closing questions and four private reminders, all draft. Exact canonical references are validated and resolved during generation; the original forty-card catalog is byte-for-byte unchanged.

Parent execution passed **13 focused tests**, including actual system Chromium interaction, not a mocked DOM. Checks cover all path content, random-pool alternatives, nested return context, zero-question release, exact opener/scroll restoration, immediate release from the path picker, literal markup/overflow, empty/single pools, safe geometry, real pointer/touch/keyboard input, and narrow viewport access. A separate delivered-artifact browser smoke check verified non-overlapping button rectangles across opener, question, experiment, picker and closing views and no console errors. Actual touch scrolling reached otherwise hidden opener text. Representative browser screenshots were visually inspected, and an independent read-only review found no browser-preview release blockers. These establish executable/browser behavior, not conversational usefulness or physical usability.

The existing V1 firmware and OTA host checks and all four existing content/prototype generator freshness checks pass. Before documentation links were added, every pre-existing tracked file matched the preserved baseline. No firmware was compiled, rebuilt, flashed, reset or otherwise contacted for this preview.

Reproduce from the repository root:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tools/build_compass_coaching.py --check
PYTHONDONTWRITEBYTECODE=1 python3 tests/compass_coaching_test.py -v
```

To deliberately regenerate the preview after an approved content/template edit:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tools/build_compass_coaching.py
```

The test harness requires local Chromium and a writable scratch directory. Tests render synthetic fixtures in disposable files and do not repair the committed generated artifact before checking it.

### Visible compromise to review

The original large-button study left a 120x100 opener viewport. The compact 22px line-icon/14px label revision increases the browser opener viewport to **144x120 pixels**, retaining invisible 56x48 side targets and 72x48 top/bottom targets. Body copy stays at 20px rather than being reduced; longer openers require scrolling, with an explicit cue and an external complete-copy inspection view. This is not yet a proven glanceable interaction. The full-width path question view is less constrained.

On the opener, **Options** supplies Another opener, Guide me and Next step/Release. Within a path, **Paths → Opening** returns to the exact previous opener without drawing again; the path picker also offers Next step and Back. Returning to a path preserves its most recent question/experiment. These routes resolve navigation gaps found during parent review, with actual failing and passing browser tests retained privately.

Do not port the cramped ring layout to hardware merely because geometric tests pass. Review whether a full-width opener with a separately requested path chooser would better return attention to people; that alternative has not been implemented or approved by this checkpoint.

## Later integration gate

The initial native integration has now been separately approved, installed and startup-verified. Further changes should continue to reuse existing V3 display/touch/reveal components and the small isolated coaching state model. Keep the current card model and accepted gesture math/range unchanged. Gate motion draws to the opener context, preserve the original experience as a fallback, and run existing V1/V2 and native V3 regressions before any separately authorized device update.
