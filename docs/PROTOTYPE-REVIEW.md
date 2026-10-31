# Read-only prototype review

This review covers the first eight-card round browser preview, now the V3 layout reference, not V2 portrait readiness, firmware or hardware readiness. Current V2 is the 172x320 portrait ESP32-C6 and needs a separate layout preview. All paired guidance is draft. Team selection uses synthetic labels; recording and saving must remain unavailable. The preview must never claim that it stored a team intervention.

## Open it

Download `prototype/index.html` and open the saved file in a browser. It is intended to be a single portable artifact with embedded content, styles and code, without a server or live AI connection. Viewing the HTML file on GitHub shows its source; it is not a hosted application.

From a local checkout, open `prototype/index.html` directly. To rebuild or check its generated content from the repository root:

```sh
python3 tools/build_prototype.py
python3 tools/build_prototype.py --check
```

The canonical editorial source is [content/cards.json](../content/cards.json). The generated file must include exactly the representative subset, not a separate hand-maintained deck or fabricated card IDs.

## What to review

The round viewport models the proposed 360x360 display. Any surrounding controls or instructions are review tools, not extra device controls. Browser pixels do not establish physical text size, bezel clearance or reliable touchscreen input.

The eight paired intents are:

- Explore: tg-001 SCOUT.
- Clarify: tg-002 FOG AHEAD.
- Experiment: tg-004 SMALL STEPS.
- Verify: tg-003 CHECK BEARINGS.
- Reflect: tg-011 MAKE CAMP.
- Unstick: tg-028 NAME THE BLOCKER.
- Include: tg-031 QUIET FIRST.
- Decide: tg-036 CHOOSE THE COMPASS.

Each has an Ask question, observable Notice guidance, optional follow-ups, and a Try invitation with its own Notice/follow-ups. Try action/activity still comes from the original card. Same intent does not mean duplicate cards.

## Short facilitator walkthrough

Try each task without reading the implementation first:

1. Start a question without selecting a team or theme.
2. Find what to listen for. Return to the same question without drawing another.
3. Open that question's related activity and return to its question.
4. Get a different question while remaining in Ask.
5. Choose a theme, return, and clear the filter.
6. Select a synthetic team and check that the card/mode does not change.
7. Find where you would record or save, and verify that neither action claims success.
8. Find Team Trail and check that it honestly explains its unavailable read-only state.

Then use the separate review fixtures to examine long text, no eligible cards and an unavailable related face. Fixtures are synthetic test states, not additions to the forty-card catalog.

For long text, read the whole question/invitation and guidance using visible controls. Watch for missing words, tiny fonts, misleading ellipses and pages that cannot be reached without a gesture. Back must preserve the card, mode and filter; it must not silently draw again.

## Useful feedback

- Which label or action did you have to search for?
- Did anything happen that you did not expect?
- Was the question easy to read before opening guidance?
- Did Notice help you listen, or did it feel like a diagnosis or script?
- Did you feel pushed to offer an activity when listening was enough?
- Which secondary action deserves easier access, if any?

Report the card ID, screen, steps and what you expected versus what happened. Avoid real participant details, team conversations or private prompts. The prototype captures none of those responses.

## Technical checks before publication

The development record must report the actual checks run, not treat this list as completed merely because it exists:

- Fresh opening shows Home and does not draw automatically.
- Main content and controls remain inside the circular visual and input boundary.
- Guide me, related-face navigation and Back preserve the correct context.
- Per-mode/filter pools handle full cycles and one-card themes honestly.
- Re-selecting a theme does not consume an extra draw; changing it consumes one.
- Recording/saving are disabled and there is no persistent storage or fake success.
- Long-text pages expose complete copy; empty/blocked-face fixtures remain honest.
- Press a button, move outside the circle, return to the starting point and release: the gesture must not activate. Check real pointer movement, not only events dispatched directly onto the button.
- A narrow browser viewport does not clip the device or shrink it without an explanation.
- Generated HTML matches the canonical data and template; all existing content and V1 tests still pass.
- Console errors and actual browser interactions are checked separately from structural/unit tests.

Evidence and unresolved limitations belong in [BRINGUP.md](BRINGUP.md). Current interaction requirements remain in [ROUND-UI-SPEC.md](ROUND-UI-SPEC.md) and [FACILITATOR-MODES.md](FACILITATOR-MODES.md).

## Initial scripted review evidence

The development host ran eleven Python tests covering content preservation, draft-only builder validation, safe/reproducible embedding and actual Chromium flows. Browser checks exercised all eight canonical paired cards in both modes, guidance pagination, pool behavior, disabled recording/saving, fixtures, button fit, circular envelopes and return context. Actual viewport checks covered 320, 360, 390 and 1280 pixels without shrinking the logical device canvas. Real mouse regression checks include leaving the circle and returning before release; normal taps and Enter activation still work.

The existing C++ firmware and OTA suites passed independently. These checks do not constitute facilitator approval, physical touchscreen testing, battery/runtime evidence or production storage validation. The initial browser review found and corrected return-context, narrow-layout, metadata-validation and drag-cancellation defects before publication.

## What comes next

Facilitator review comes before enriching all forty cards. Production eligibility, mode-aware pools, events/saves, migration, durable storage and actual-device touch/display checks remain separate implementation gates. Keep the working V1 fallback unchanged while reviewing this preview.
