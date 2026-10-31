# V3 round-screen UX and UI specification

Scope: the existing circular preview is retained for future V3. Current V2 is the portrait ESP32-C6 board described in [V2-SPEC.md](V2-SPEC.md); it requires a separate layout, not a shrunken circle.

Status: an initial eight-card read-only browser preview now exists at prototype/index.html; the design below remains subject to facilitator and actual-hardware review. Production firmware/tracking is not implemented. Geometry and browser layout checks do not establish physical readability, touch accuracy, bezel clearance, memory use or device performance. Numeric budgets remain review inputs.

Canonical requirements: [PRODUCT-SPEC.md](PRODUCT-SPEC.md), P-17. Behavior/content/tracking contract: [FACILITATOR-MODES.md](FACILITATOR-MODES.md), P-16. Hardware gates: [V3-SPEC.md](V3-SPEC.md). Sequencing: [PLAN.md](PLAN.md).

## 1. Design decisions

- Start with **Ask the Team** and **Try a Move**. Fresh opening/reboot lands on Home with Ask visually prioritized; it does not automatically draw a card. Tapping either mode performs the draw. Returning to Home alone neither draws nor resets existing pool states.
- The question or action is the focal point. Theme, mode and team are context; the full card title moves into Guide me rather than competing with the question.
- The main card has two large bottom actions: **Guide me** and **Another**. Back, team and More stay visible above it. Keep recording, saving and other navigation out of the main text area.
- **Guide me is the only help entry.** Say, Notice and optional Follow up are core. Later Steps, AI prompt and tool notes extend that same view; there is no separate Show me how button.
- Use **Asked this** in Ask and **Offered this move** in Try. The latter means offered/started, not completed successfully. Team Trail may retain the short Asked/Tried labels with this distinction explained in event detail.
- Touch is sufficient for every action. No required swipes, long presses, hidden edge taps, shake or automatic movement.
- Keep the compass theme through restrained styling. A decorative needle must not take space from a question or imply actual magnetic heading.

## 2. Circular geometry and proposed layout budget

Prototype canvas: 360 x 360 logical pixels, center (180, 180). Start with a safe circle of radius 168, a 12-pixel radial inset from the nominal display edge. This is an engineering assumption, not a measured bezel allowance. Actual hardware may require a larger inset.

Use the middle of the circle, not the width of the square bounding box. Apply both a visual clip and an input/hit-test mask: off-circle or out-of-safe-region decorative areas cannot trigger controls. Each rectangle below was checked at all four corners against the safe circle. Shadows, focus rings, scroll indicators and text padding must also stay inside it. Keep at least an 8-pixel gap between separate targets in these initial layouts.

### Main card coordinates

Coordinates are x, y, width, height in logical pixels:

- Back target: 70, 64, 64, 48.
- Team target: 148, 64, 64, 48.
- More target: 226, 64, 64, 48.
- Context label: 70, 116, 220, 22. Example: Ask / Verify; review prototypes also show Preview here.
- Question/action viewport: 70, 142, 220, 78.
- Guide me target: 70, 232, 104, 52.
- Another target: 186, 232, 104, 52.

The widest header corners are at radius 159.86, inside the proposed radius 168. The two bottom-button rectangles reach radius 151.38. These calculations establish geometric containment only.

Initial typography budgets:

- Main question/action: 20-pixel font, 26-pixel line height; three visible lines in the 78-pixel body viewport.
- Context: 14-pixel font, 20-pixel line height.
- Button labels: 16-pixel font, measured and centered inside padded targets.
- Guide me body: start at the main text size; do not solve overflow by making guidance tiny.

These are logical-pixel targets, not claims of physical touch size or accessibility-standard compliance. Measure the actual rendering and usability on the device. Do not infer font fit from average characters per line.

### Home coordinates

Keep the same safe-circle envelope:

- Ask the Team: 70, 124, 220, 56.
- Try a Move: 70, 188, 220, 56.
- Team Trail, secondary: 96, 252, 168, 48.
- Team selection remains available in the header; no setup screen blocks a first draw.

Branding is optional small context, not an extra button or an animated splash that delays use. Home mode actions start a new unfiltered draw. Back from a root card returns Home; returning through Home is not linked-face navigation.

### Paged menu/list coordinates

Show at most three choices per page:

- Row 1: 70, 128, 220, 48.
- Row 2: 70, 184, 220, 48.
- Row 3: 70, 240, 220, 48.

The lower row reaches radius 154.16. Keep visible Back and a labeled page control in header targets; show the page number in the context label. Do not require scrolling to discover hidden menu choices. Disabled controls remain visibly disabled and do not wrap pages silently.

## 3. Primary flow and navigation cost

### Main card

Header: Back / team code or No team / More. Context: mode and theme. Main body: the question or action. Footer: Guide me / Another. A full card title and stable reference are available inside Guide me and record/event details.

- Guide me: one tap from the card.
- Another question/move: one tap; stays in the current mode/pool.
- More: opens a paged action list, not a small popup over unreadable text.
- Team: one tap opens team selection. Use a stable short team code if the full label cannot fit; show the full label and code before confirming attribution.

Team switches do not draw a new card or change mode. Back from a picker without choosing changes nothing. Preserve current mode, filter, card revision, guide position and originating context explicitly; a generic back stack must not recreate random cards.

### More, first page

1. Asked this / Offered this move, according to current mode.
2. Save / Saved.
3. Try this move / Ask about this, opening the same intent's eligible other face.

Header More advances to the second page; Back returns to the card. Context shows Actions 1/2. This puts same-intent mode switching two taps from the main card without a permanent extra button.

### More, second page

1. Themes.
2. Team Trail.
3. Home.

Context shows Actions 2/2; a labeled First control returns to the first page. Theme selection is intentionally secondary: main card -> More -> second page -> Themes -> choice. Test whether that extra navigation is acceptable before adding another permanent control.

Theme list: All themes plus eight existing themes, three choices per page. Selecting a different theme performs one draw from that mode's eligible pool. Selecting the already-active filter returns to the same card without a draw. Back cancels without changing selection. Clearing a filter is explicit. Future situations and collections use a separate reviewed navigation extension, not extra home buttons.

### Related-face behavior

A same-card face change preserves ID/revision and does not consume a draw. It is offered only when that face is release-eligible, or explicitly draft-labeled in a review prototype. If unavailable, show a disabled row with a plain explanation; do not substitute an unrelated card or silently display draft content.

Independent mode pools may revisit the linked card on a later random draw. Prefer clarity over silently changing pool-consumption rules; test whether the behavior surprises users and revise the requirement explicitly if necessary.

## 4. One help entry and progressive guidance

Guide me reuses the current card and mode; it does not draw, save or record. Its context label shows a compact card reference, section and page position. The Say section contains the full title as a secondary body heading and the complete spoken text, with additional pages if needed. Do not squeeze a full title plus reference into the single context line or shrink either to make it fit.

Core sections:

- Say: an open question in Ask; a human invitation in Try.
- Notice: observable cues, not a diagnosis or test score.
- Follow up: one optional question per page, with permission to pause or stop.

Use visible Previous and Next page targets in the bottom two-button layout. Header Back always exits to the same card, not the previous guidance page. Display section name and position, for example Notice 2/4. The page count includes overflow pages, not just section names. On the first/last page disable unavailable navigation clearly.

Do not present Say, Notice, two follow-ups, Steps, AI prompt and two tool notes on one screen. Later depth lives under an explicit More guidance section inside Guide me; it is not appended to every core path. Only show content that exists and is eligible. No automatic branch selection based on a team's answer.

Guide me never starts the related activity. The optional invitation offers a human choice; the related-face action remains an explicit navigation operation. Back preserves the card and its text position where practical.

## 5. Long text, input and accessibility behavior

### No silent clipping

- Prefer an edited main question that fits the measured three-line budget without shrinking the font. Preserve the longer reviewed wording where needed in Guide me.
- Do not create an unreviewed shorter paraphrase at runtime. A separately authored display summary would require a reviewed content field and explicit schema decision.
- If approved main copy overflows, allow vertical scrolling inside the fixed body viewport with a visible continuation cue and scrollbar. Guide me remains a visible tap route to the complete Say text without requiring a gesture.
- Guidance may split across pages at sentence or word boundaries. Never discard a word, truncate an essential qualifier or show an ellipsis as if the whole instruction were present.
- Long unbroken text wraps safely or fails content/layout validation; it must not draw outside the circle.
- Do not move bottom actions while body text scrolls. Reset only the intended text position on a new card, not the team or mode.

### Input safeguards

- Activate buttons on a valid release within the target; a drag/scroll must not become a tap.
- Initial prototype drag threshold: 8 logical pixels. Treat this as a tuning parameter, not hardware-tested behavior. Once a scroll begins, do not activate a button on release.
- Block rapid duplicate draws and duplicate confirmations at the state/action level, not only by adding a delay.
- Freeze confirmation context to card ID/revision, team and mode; reject stale confirmation after navigation or a context change.
- Wake consumes the first touch. No recording, drawing or menu activation on wake.
- Disable shake by default in the first UX prototype. Later shake is optional and limited to the main card screen; suppress it during scrolling, animation, menus, guidance and confirmations.
- The first prototype uses no decorative spin animation. Add it later only if it does not delay access to the question or queue draws.

Use high-contrast text, text labels rather than color-only meanings and visible focus/pressed/disabled states. No flashing, audio dependency or required motion. On-screen fit does not prove readable text under workshop lighting or accurate taps; these need human checks.

## 6. Recording, saves and history without clutter

These are planned production paths, not enabled behavior in the first read-only prototype.

- More -> Asked this / Offered this move opens a dedicated confirmation showing full team label/code, mode and card identity. With an active team, confirmation is three taps from the card. No-team recording adds an explicit picker and a new confirmation; it never invents a team.
- Bottom Cancel / Record use the two-button geometry. Record means the stated intervention happened, not that it succeeded. A write failure shows an error and preserves safe retry; no success message before durable commit.
- The confirmation remains valid only for the frozen context. Double taps append one event; explicit reuse starts a new pending action.
- The action page shows Saved if that scope/card is already saved. Opening it in the other mode does not silently change the original saved face. Unsave is an explicit separate action, not an accidental consequence of revisiting Saved.
- Ask and Try events can both exist for one card. Team Trail labels them Asked/Tried, with exact meaning in the event detail and no implication of learning or completion.
- History rows use mode, short title and order. Open event detail for full wording, mode meaning, feedback and Undo. Do not squeeze a dashboard, charts and edit controls onto a circle.
- Reopened saves and related faces obey current eligibility; historical recorded snapshots may still be displayed as clearly labeled history without making them eligible for new use. Unsupported or retired content has an honest unavailability state.

Full migration, privacy and durability behavior remains in FACILITATOR-MODES.md and PRODUCT-SPEC.md. The round UI does not weaken those contracts.

## 7. State and return-context contract

Plan separate states: Home; ModeCard; GuideMe; ActionPage; ThemePage; TeamPage; RecordConfirm; SaveDetail; TeamTrail; EventDetail; EmptyPool; StorageError; Dimmed. Do not duplicate the whole implementation by mode.

Capture mode, card ID/revision, active pool/filter, short/full team identity, text/guide page position and originating route in an explicit view model. Navigation events do not append use events. A related-face event does not mutate deck state. A theme change draws exactly once only after an actual changed choice.

In the first read-only prototype, team context is a visible synthetic fixture only; recording/saving are clearly disabled. A read-only eight-card prototype with one card per theme must honestly explain when a one-card pool repeats. It does not pretend to prove full-deck shuffling.

## 8. Prototype-first implementation plan

### UX-A: eight representative paired cards

Author draft Ask/Notice/Follow-up and Try guidance for these existing intents, preserving base actions and IDs:

- Explore: tg-001 SCOUT.
- Clarify: tg-002 FOG AHEAD.
- Experiment: tg-004 SMALL STEPS.
- Verify: tg-003 CHECK BEARINGS.
- Reflect: tg-011 MAKE CAMP.
- Unstick: tg-028 NAME THE BLOCKER.
- Include: tg-031 QUIET FIRST.
- Decide: tg-036 CHOOSE THE COMPASS.

These eight pairs are now authored as draft fields in canonical JSON and the readable catalog. They remain prominently draft-labeled in the preview; authoring and structural tests do not constitute editorial approval. Include a separate synthetic overflow fixture so long-copy handling is exercised even if the initial examples fit.

### UX-B: read-only interactive round prototype

The initial portable browser artifact is built; use the [review walkthrough](PROTOTYPE-REVIEW.md) and verify regenerated output with tools/build_prototype.py --check. Its review scope is Home, Ask/Try card, Guide me pages, related faces, More pages, theme pages, empty/ineligible states and a synthetic team-context picker. Team Trail shows a clearly labeled static preview or unavailable message; no recording, saving, storage migrations or fake success messages.

Initial implementation adjustments for review: menu context is compacted to y=111 to leave space above the first choice row; the full title gets a separate Guide page; overflow uses a visible Full text: Guide me route instead of scroll-only access. Narrow viewports keep the 360-pixel canvas and show an explicit horizontal-scroll explanation instead of silently shrinking it. These are prototype-tested choices, not settled physical-device design decisions.

Validate exact-circle clipping, font metrics, button/scroll separation, Back behavior and all long-text/empty states. Keyboard/mouse preview aids do not prove touchscreen accessibility. This is workflow review, not actual hardware execution.

### UX-C: facilitator review and revision

Without coaching, ask the reviewer to:

1. Start with a question without selecting a team/theme.
2. Find what to listen for, then return without losing the card.
3. Open that intent's activity without drawing a different card.
4. Draw another question in the same mode.
5. Choose a theme and clear it again.
6. Read the complete overflow fixture without mistaking it for a complete short question.
7. Find where Asked this / Offered this move will live, and explain why they are disabled in preview.
8. Identify the active team and understand that it changes attribution, not advice.

Record observed mis-taps, route confusion, unreadable text and unprompted completion. Do not infer success from the designer explaining the interface. All tasks must be possible via labeled taps. Revise before expanding paired guidance to forty entries.

### UX-D: implementation gates after review

Then complete content/editorial review; implement and test eligibility and pools; implement mode-aware events, saves and migration; enable tracking only after durability tests; integrate on inspected V3 hardware; verify real display/touch, memory budget, soak and OTA retention. Preserve V1 throughout. A successful prototype does not skip these gates.

## 9. Acceptance and evidence

- Geometric: every text/control/focus/scroll boundary fits the actual safe circle. Current coordinate assumptions are tested independently of text quality. Test outside-circle/corner taps explicitly: they do nothing even if the prototype's underlying window is square.
- Readability: representative shortest/longest text fits measured fonts or uses an explicit complete-content route. No automatic font reduction, essential ellipses or clipped touch labels.
- Touch: proposed 48-pixel minimum target height and 52-pixel primary controls are measured and tuned on the actual device; their initial sizes are not declared sufficient by calculation alone.
- Navigation: Guide me and Another are each one tap; same-card face switch is two taps; active-team confirmation is three taps to commit. Recheck those paths after any redesign.
- Safety: Back, scrolling, guidance, face switching, canceled selection, wake and disabled-state shaking do not draw unexpectedly, record use or overwrite context.
- Eligibility: test linked faces, saves, empty pools and unavailable current revisions, not only random draws. History is not a back door into publishing retired content.
- Evidence: label outputs Planned, Prototype-tested or Hardware-verified as appropriate. Keep synthetic preview data separate from real private Team Trail data.

## 10. Deferred choices

Final fonts, colors, physical target sizes, touch thresholds, animation duration and bezel allowance remain open until measured review. Alternative faster theme access is a prototype question, not permission to crowd the root card. More themes, a radial pie menu, always-visible usage controls, on-screen typing and extra home shortcuts are not initial scope.
