# TrailGuide — current project and system specification

## 1. Purpose and authority

TrailGuide supports **project-led mentoring for individuals and teams**.
Start with something meaningful to make, investigate or improve, then choose the
next useful milestone and appropriate assistance. Learner independence means
being able to continue with suitable support, not compulsory unaided work.
Researching or building with AI can support learning even if the artifact contains no AI.

The working learner loop is **Define → Direct → Check → Adjust**.
Use it repeatedly within meaningful project work, not as a mandatory tool ladder.
Choose human, AI-assisted or hybrid routes according to purpose, access and risk.
Plan useful checks during Define; AI/tools can help produce evidence and run checks.
Human judgment remains responsible for purpose and consequential acceptance.
Passing, asking for direct help, seeking expertise and choosing non-AI routes are valid.

This document is the concise **current baseline**, not a replacement requirements set.
Document responsibilities are:

| Document | Authority / use |
| --- | --- |
| [PRODUCT-SPEC.md](PRODUCT-SPEC.md) | Canonical stable P-ID product requirements and explicitly planned scope. |
| [CLASSIC-FLOW.md](CLASSIC-FLOW.md) | Current Classic interaction diagram. |
| [COACHING-FLOW.md](COACHING-FLOW.md) | Current Coaching interaction diagram. |
| [V3-SPEC.md](V3-SPEC.md) | Round-target hardware contract and target-specific acceptance. |
| [V3-TRIAL.md](V3-TRIAL.md) | Installed trial behavior, execution evidence and remaining gates. |
| [HANDOFF.md](HANDOFF.md) | Current checkpoint and continuity; older sections are historical. |

Read installed-checkpoint evidence alongside requirements: planned Home, filters,
tracking and update features are not automatically features of the running trial.
Older version-specific paragraphs do not supersede the newer installed checkpoint.

## 2. Project deliverables and boundaries

The primary work is mentoring practice, reusable materials and learner-owned context
for continuing with AI or human support. See [COACHING-DIRECTION.md](COACHING-DIRECTION.md)
and the [portable project/coaching brief](PROJECT-COACH.md).
These are drafts for use and review, not a validated curriculum or integrated AI tutor.
Voluntary feedback and project evidence are needed before claims of learning benefit.

The physical companion is an optional enjoyable memory cue and building experiment.
Its small display and playful reveal do not define the curriculum or limit all materials.
A mentor chooses, adapts or ignores prompts; the device does not conduct the conversation.
There are no required question counts, experiments, AI-use targets or inferred diagnoses.

## 3. Current system checkpoint

The round device runs **v3-0.1.14 DRAFT**, installed with application-flash comparison
and exact-version startup, initialized UI and real-IMU liveness verification recorded.
This is an offline, read-only draft-content trial, not the broader product release.
No new hardware contact or firmware test is implied by this documentation baseline.

The target is the original Waveshare ESP32-S3-Touch-LCD-1.85 Rev2.1:
360x360 round capacitive touch, ESP32-S3, 16MB flash, 8MB PSRAM and QMI8658 IMU.
Detailed pins, dependencies, recovery and deployment constraints remain in
[V3-SPEC.md](V3-SPEC.md) and [V3-BRINGUP.md](V3-BRINGUP.md).
The gyro remains configured at ±2048 dps; the coaching presentation did not retune sensing.

V1 remains a separate twelve-card OLED/button fallback; V2 remains a separate portrait
ESP32-C6 trial. Their firmware, pin maps, layouts and evidence are not interchangeable.
Authenticated local V2 OTA is separate from V3: **V3 OTA is not implemented**.
See [V1-SPEC.md](V1-SPEC.md), [V2-SPEC.md](V2-SPEC.md) and [V2-TRIAL.md](V2-TRIAL.md).

## 4. Classic mode: current behavior

- Fresh boot opens the plain decorative wandering compass, without an automatic draw.
- The compass is an idea metaphor, not a magnetic heading or navigation instrument.
- Tap the compass for Options: **Coaching**, **Back**, **Ask the Team**,
  **Try a Move** and **Instant text** are the touch choices.
- Ask and Try draw from the same selected twelve curated paired draft cards.
  Ask presents a question; Try presents an action. They have independent shuffled bags,
  with no repeats within a bag cycle and no immediate repeat at its boundary.
- A card offers **Guide me**, **Another**, **Home** and **More text** after the reveal/hold.
  Another draws in the current Ask/Try mode. Guide me adds mode-specific help;
  Back returns to the same card without drawing. More text/scroll exposes complete copy.
- Home returns to the compass without resetting the model's stored Ask/Try mode.
  However, the current Home gesture route explicitly draws Ask; settled-card gestures
  use the active mode. Do not infer a Try-on-Home gesture from retained model state.
- A completed eligible turn-over-and-back selects one new card; shake is not required.
  Touch Ask/Try/Another provides a route without motion sensing.

The installed selection is not the entire forty-card catalog and is not production
review eligibility, theme filtering, linked-face navigation or durable use tracking.
See the [Classic flow](CLASSIC-FLOW.md) and [trial contract](V3-TRIAL.md).

## 5. Coaching mode: current behavior

Enter through **compass → Options → Coaching**. Classic remains the boot default.
The first entry selects an opening question; subsequent entries within the same boot
restore the remembered opener context rather than requiring a new opener.
Coaching **Options → Classic** returns to the plain Classic compass.

The authored content in [content/coaching.json](../content/coaching.json) contains:

| Content | Current count / order |
| --- | --- |
| Opening questions | 6, independent shuffled pool with boundary-repeat avoidance. |
| Paths | Progress, Stuck, Unclear, Exploring, AI Results, Team. |
| Path questions | 4 per path, 24 total. |
| Optional experiments | 3 per path, 18 total. |
| Closings | 3 optional next-step prompts. |
| Private reminders | 4 listening cues. |

There are **55 entries** in total. Questions, experiments, closings and reminders
cycle in authored order on explicit Another/Next actions; they are not random draws.
Path labels may be shortened on the perimeter, but headings retain their full names.

- Opening presents the six path controls and a plain text **Options** touch entry.
- Options offers **Another opener**, **Guide me**, **Release**, **Classic**,
  **Instant** and **Reminders**, with Back to the previous context.
- **Another opener** in Options requests a new random opener by touch. A motion
  request works **only while on Opening**; path browsing cannot be replaced by it.
- A path restores its last question/experiment face. Question faces offer **Paths**,
  **Guide me**, **Release**, **Another** and **Try (optional)**; experiment faces offer
  **Question** to switch back without consuming a fresh opener.
- The path picker permits free path changes, return to **Opening**, or **Next step**.
  Release/Next step opens an optional closing without a quota or required experiment.
- Guide me adds private guidance for the current entry; Reminders are private cues,
  not automatic notifications or learner assessments.
- Opening returns to the exact remembered opener. Path question/experiment selection,
  last face and scroll context are kept independently in RAM.

No answer determines a path automatically. The mentor controls every question,
experiment and release, and can adapt, skip or stop. See [COACHING-FLOW.md](COACHING-FLOW.md).

## 6. Presentation, input and accessibility

The approved narrower coaching opening face is **192x112 pixels**, inside the unchanged
**256x140 snapshot**. Coaching body text is **18px**, opening wrap is **168px**,
and perimeter captions/plain opening Options are **12px**. Other navigation captions
remain distinct; these dimensions do not redefine Classic's 20px main card typography.
Compact outward line icons retain labels and generous touch targets.

Both draw experiences retain the existing whole-face **six-second reveal**, followed
by an **additional five-second settled reading hold**. The answer-bearing surface
and words move together; this is not typing or live generation. Initial Coaching
entry restores/renders context; requesting a new opener invokes the draw/reveal path.
Tap during reveal finishes the same selected content; tap during the settled hold
exposes controls early. Instant text bypasses animation; complete copy remains accessible.
Capture failure falls back to instant complete text rather than claiming working motion.
Guide/Back navigation stays instant. Dragging/scrolling must not act as a reveal tap.

Qualified inversion blanks the previous eligible scene without drawing; completed
return draws once, and cancellation restores the previous scene. Reveal-busy input
is not queued. The settled hold is separate from reveal-busy: a new eligible complete
turnover may draw during that hold. Touch, menus, guidance and scrolling suppress motion.

The preceding v3-0.1.13 icons and spacing received positive physical feedback.
The v3-0.1.14 side trim was approved in native preview; its new physical acceptance,
broader coaching touch/readability and natural-opener gesture checks remain pending.
Host geometry and rendered fit do not establish physical usability or reliability.

## 7. Content and software architecture

[content/cards.json](../content/cards.json) is the canonical forty-card editorial catalog:
eight themes, five cards each, stable IDs and revisions; published review states remain draft.
V3's generator selects twelve paired Ask/Try cards without changing V1/V2 selections.
Coaching JSON is a lightweight authored layer; canonical card references are resolved
during generation rather than copied into a second authoritative card catalog.

The implementation pipeline is:

1. Canonical cards/coaching JSON → content generators under `tools/`.
2. Generated `cards_generated.h` / `coaching_generated.h` → pure selection/state models.
3. `model.h` / `coaching_model.h` → `ui.h` / `coaching_ui.h` navigation and presentation.
4. Gesture eligibility and shared `WindowReveal` → pinned LVGL 8.3.10 and board drivers.

Mode, pool, opener, path and scroll state are **RAM-only**. Returning to Classic and
re-entering Coaching within a boot preserves opener context; reboot persistence is not promised.
Stored draft text runs offline. There is no live AI, learner audio, conversation capture,
scoring, saving, use recording, team roster, Team Trail/history, cloud sync or V3 OTA.
The default-OFF bounded technical motion recorder is diagnostic RAM, not learner history.
Keep private diagnostics, credentials, device identifiers and runtime data out of public docs/builds.

## 8. Evidence, remaining gates and roadmap

The last firmware checkpoint records full actual native LVGL 8.3.10 ASan/UBSan checks,
80 isolated Python tests, both V1 suites, all six generator checks, S3/package checks,
application-only write/explicit flash comparison and exact-runtime readback passing.
These are recorded firmware results, not tests newly run for this document.
Earlier natural Classic gesture observations do not prove new Coaching physical acceptance.
Broader handling, endurance, power, memory and learner-benefit claims retain separate gates.

Team Trail, durable history/saves, filters, production eligibility, broader Home designs
and richer integrations are **unimplemented roadmap**, not prerequisites for mentoring
or capabilities of this installed trial. Their stable requirements and future acceptance
remain in [PRODUCT-SPEC.md](PRODUCT-SPEC.md); this baseline adds no feature commitments.
