# V3 twelve-card content review

Status: **implemented and installed in V3 v3-0.1.8 DRAFT**, with exact-version startup/UI/IMU readback. All forty catalog entries and paired scopes remain `draft`; wording endorsement is not production eligibility, learning evidence or physical gesture acceptance. V1/V2 installed artifacts are unchanged.

## Scope

V3 selects twelve **existing** canonical IDs in the order below. No titles, themes, legacy `question` fields or agent-workflow variants change. The other twenty-eight cards are unchanged. Each of these twelve revisions advances exactly once. Four cards gain paired facilitator fields; their new Try invitations/observation cues remain draft and retain the existing practical activity.

The main Ask surface shows one possible opening. Guide adds a private listening cue, optional follow-up, the conditional unhelpful-output question where applicable, and optional help **after listening**. No AI offer is inserted into a discovery question. The mentor can use, adapt or skip the prompt, listen without offering anything, or leave the learner to try their chosen next step. Use only appropriate, non-sensitive material with any optional AI tool. The device does not collect the conversation or supply a contextual diagnosis.

The base `question` remains a separate legacy discussion prompt. For rows without an earlier paired Ask, the Before column identifies that legacy prompt rather than pretending a paired Ask existed.

| ID / title | Revision | Before: paired Ask (or legacy question) | New paired Ask |
| --- | --- | --- | --- |
| tg-002 / FOG AHEAD | 2 → 3 | What are you trying to make easier, and for whom? | What are you hoping to accomplish? |
| tg-001 / SCOUT | 2 → 3 | What different approaches have you considered? | What different approaches have you considered? |
| tg-004 / SMALL STEPS | 3 → 4 | What is the smallest thing you could try to learn more? | What would be useful to learn from your next attempt? |
| tg-003 / CHECK BEARINGS | 3 → 4 | Which part would matter most if it were wrong? | Which part would matter most if it were wrong? |
| tg-011 / MAKE CAMP | 3 → 4 | What changed in your approach after that attempt? | What did you notice in that attempt? |
| tg-028 / NAME THE BLOCKER | 2 → 3 | Where did progress stop? | What have you tried so far? |
| tg-031 / QUIET FIRST | 2 → 3 | What do you notice before seeing someone else's answer? | How is working together going for you? |
| tg-036 / CHOOSE THE COMPASS | 3 → 4 | What matters most in choosing your next step? | What matters most in choosing your next step? |
| tg-010 / SHOW THE TARGET | 1 → 2 | Legacy: What makes this example useful rather than merely polished? | What makes this example useful for what you want to do? |
| tg-012 / CHANGE COURSE | 1 → 2 | Legacy: What would a thoughtful critic challenge first? | What concerns, if any, do you have about this plan? |
| tg-014 / HUMAN FIRST | 1 → 2 | Legacy: What would we do if AI were unavailable? | How would you approach this without AI? |
| tg-026 / KEEP OR ADAPT | 2 → 3 | Legacy: What is worth repeating, and what should change next time? | What is working well that you would like to keep? |

Unchanged Ask wording still receives the one revision increment because its private cue/follow-up/help changes. Purpose, reflection, collaboration and strengths are openings, not a reinterpretation of the stable title as a diagnosis. Try remains an optional practical activity, not a mandated answer to Ask.

## Two narrowly scoped base adjustments

- **CHECK BEARINGS:** action and legacy question are unchanged. Activity no longer requires checking independently *of AI*. It permits AI/tool assistance anchored in original sources, observation or trusted references, requires actual evidence rather than model agreement, and retains the unverified/risky-action safeguard. The optional AI prompt likewise permits locating support or running checks and forbids invented results. Existing Try observation/follow-up and agent variant remain unchanged.
- **HUMAN FIRST:** action replaces the required AI-help step with a complete human-only choice. Activity retains the three-step human-only workflow, makes comparing AI-assisted/hybrid routes optional, and includes preparation, checking, corrections, learning value and accountable judgment. The optional AI prompt remains unchanged because it is only a voluntary separate template. No other card's base action/activity/AI prompt changes.

## Generation and compatibility

`tools/build_v3_content.py` owns V3's twelve-ID selection. Shared validation rejects missing/duplicate IDs, malformed optional fields, invalid review scopes and selections outside one to forty entries. `trailguide::cardCount` derives from the generated array and sizes V3's independent Ask/Try bags. Empty/single generic model tests guard modulo-zero and boundary loops; the safe maximum is forty. Shuffling retains rejection-sampled Fisher–Yates and boundary conditioning without an adjacent repeat for pools larger than one. There are no filters, persistence or new modes.

The default `build_prototype.select_content` and V2 generator still select eight. V2's generated eight-card header is refreshed for **future builds only**; no V2 source UI, model, network, firmware binary or installed artifact changes. The earlier eight-card browser studies only refresh their embedded canonical JSON; their UI and pool stay unchanged and are not the native twelve-card review reference.

Canonical smart punctuation stays exact in JSON/catalog. The unchanged embedded fonts use explicit punctuation-only mapping (`’` → `'`, curly double quotes → straight quotes, `—` → `--`); unsupported characters still fail generation. All twelve main openings already use ASCII, so the device opening wording is exact without mapping. No font, geometry, six-second whole-face reveal or five-second card-only hold changes.

## Verification boundaries

Focused content/generator checks, fair-model sanitizer tests, and actual pinned LVGL typography/render regressions are required before integration. `tests/v3_content_ui_test.cpp` checks all twelve IDs in Ask and Try, main and Guide (48 contexts), the complete overlapping scroll route and available glyphs, unchanged 20/18px fonts and safe-circle geometry. It exports native framebuffer pages for review. The existing renderer suite checks whole-copy reveal, hold, navigation, allocation failures and synthetic input; its bag loops now derive from generated count and cross the expanded boundaries.

The merged release passed all four generator freshness checks, 45 isolated Python checks, actual LVGL ASan/UBSan, model/SDK/recorder regressions and independent cross-integration review. Actual S3 compile/image/source/backup/target checks, app-only write/hash and exact-version startup/UI/IMU readback passed. On the real target, an explicitly synthetic detector-state injection selected tg-004; its ROM opening matched exactly, and actual reveal/settled-hold/control-restoration state completed. A 1,904-record CRC-verified capture retained exactly one synthetic trigger. The board was intentionally reset to Home after exporting evidence. This is not visual pixel or physical flip acceptance, measured FPS or a learning outcome.
