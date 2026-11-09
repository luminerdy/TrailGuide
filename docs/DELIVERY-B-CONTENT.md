# Delivery B — historical seven-field wording delivery retained in baseline28

## Current frozen working baseline — v3-0.1.28-controls

**`v3-0.1.28-controls` is the current frozen working source baseline and future development reference in this checkpoint, matching the installed DRAFT trial—not stable-production certification.** Both Classic and Coaching retain a **six-second whole-face reveal**, followed by a **two-second settled card-only hold** before controls return. Touch can finish the same reveal/start its hold, then expose controls; dragging is not a tap. Classic remains the startup default; shared RAM-only sound defaults **OFF**. The optional plain **S** arms an eight-second shake-intent window; ±16g sensing and global Start Fresh policy are retained, not newly repaired.

The user's qualitative **“Works Great”** feedback is positive timing acceptance for installed28. Mode and trial count were unspecified: it is not new gesture, exhaustive readability or field-reliability acceptance. **Known issues remain OPEN:** Coaching compass shaking failed in the inherited27 report; automatic shaking was intermittent after path selection, where S then shake worked. The original turnover-stuck-compass/blank cause also remains unresolved. No27/28 field movement trace attributes these failures.

See the [frozen baseline contract](BASELINE-v3-0.1.28.md) for application size/hash, source/private-artifact boundaries, build setup and freeze rules. Source publication is completed only after the integrating maintainer commits, validates and creates tag `v3-0.1.28-controls`; this edit does not claim that tagging or remote publication has happened. Private binaries, ELF/maps, captures and deployment tooling are not published. Older22/27 ledgers retain historical evidence and timing; archived recovery27 remains preserved, without automatic rollback.

## Scope and status

The exact seven-field C1-v2 sample was approved and incorporated in installed23, then retained unchanged through installed28. Its earlier candidate-on22 description is historical, not the current delivery state. Baseline28 source includes the matching authored Coaching JSON, generated consumers and editorial contracts; this documentation edit does not independently regenerate or change them. No new wording, schema or review-state promotion is included.

The forty-card canonical catalog, twelve-card Classic selection, all 55 Coaching identities and their order, eleven references, review states, paths and schemas remain unchanged. Editorial approval does **not** promote any draft review state. No gesture threshold, quiet/rearm rule, navigation policy, layout, font, reveal/hold timing, sound setting or boot default changes here.

## Exact editorial changes

Three visible prompts:

- **AI-RESULTS-Q02:** “How does the output compare with what you expected?” Allows good, mixed and disappointing results without assuming disappointment.
- **PROGRESS-E03:** “Would it help to identify one observation that could increase or decrease your confidence in the result?” Supports evidence that can move confidence in either direction.
- **TEAM-Q04:** “From your perspective, what matters most right now?” Does not ask one person to invent everyone else's position.

Four complete private Guide fields, with their visible invitations and references unchanged:

- **STUCK-E01:** one optional reversible step, expected/actual comparison, a conditional copy/mock-up boundary and conclusions limited to that step.
- **EXPLORING-E01:** a feasible human-only step is a complete choice; compare human-only, AI-assisted and hybrid routes only if useful, including checking, correction, integration and learning.
- **AI-RESULTS-E02:** check one consequential claim against original sources, executed checks or observations; retain uncertainty where evidence is missing.
- **TEAM-E01:** listen for differences without diagnosing silence or inventing agreement; preserve voluntary sharing, unexplained passing and accessible alternatives.

The exact approved before/after values and preservation hashes are exercised by [the editorial contract](../tests/coaching_editorial_test.py) and its [fixture](../tests/coaching_editorial_expected.json). Complete new guidance remains in [the authored Coaching layer](../content/coaching.json), rather than a second editable deck.

## Validation and limits

- The new editorial contract first produced seven expected failures against unchanged content, then passed after precisely the approved fields were changed.
- Existing semantic tests now recognize the approved optionality cues without requiring the literal word “optional” in every revised Guide. Human-only choice and AI-self-critique limits remain checked.
- Native C++ and standalone browser consumers are regenerated and checked against the same canonical inputs. Other generated content remains current.
- A newly compiled ASan/UBSan harness using actual pinned LVGL checks 48 Classic faces and 110 Coaching main/Guide faces. The seven changed faces receive top/bottom captures, real pointer scrolling where needed, and visible last-line endpoint checks. Original C-library objects are reused only after source/configuration/flags/object-hash verification; this is not a fresh compilation of LVGL itself.
- Native captures support text delivery, wrapping and endpoint inspection—not physical readability, learning effectiveness, ordinary-handling gesture reliability or installation acceptance. Long revised Guides still require scrolling; no smaller font or automatic abridgment is introduced.

The optional operating/mentoring reference and shared-Instant/Classic scroll-memory proposals are not included. The separately observed need for steadiness before gestures remains open; this retained wording delivery neither diagnoses nor fixes it.
