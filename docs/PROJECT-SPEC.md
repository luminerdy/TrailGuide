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

**Approved direction: Coaching-first, with revised V3 wording as its content foundation.** This is not a startup-default change or a content merge: Classic still boots by default and remains an intact optional quick-prompt route. Coaching currently uses its own authored layer plus canonical references; neither all twelve Classic pairs nor the whole forty-card catalog has been merged into Coaching.

**v3-0.1.20-shake-calibration DRAFT is installed**. This remains an offline authored-content trial, not a production release; local installation does not imply publication.
Exact-version runtime verification is recorded for this installed checkpoint; this documentation update did not contact hardware.

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
  **Try a Move**, **Instant text** and **Sound OFF/ON** are the touch choices.
- Ask and Try draw from the same selected twelve curated paired draft cards.
  Ask presents a question; Try presents an action. They have independent shuffled bags,
  with no repeats within a bag cycle and no immediate repeat at its boundary.
- A card offers **Guide me**, **Another** and **Home** after the reveal/hold; **More text** appears only while actual text remains below.
  Another draws in the current Ask/Try mode. Guide me adds mode-specific help;
  Back returns to the same card without drawing. More text/scroll exposes complete copy.
- Home returns to the compass without resetting the model's stored Ask/Try mode.
  However, the current Home gesture route explicitly draws Ask; settled-card gestures
  use the active mode. Do not infer a Try-on-Home gesture from retained model state.
- A completed eligible turn-over-and-back or sustained shake selects one new card; neither requires the other.
  Touch Ask/Try/Another provides a route without motion sensing.

The installed selection is not the entire forty-card catalog and is not production
review eligibility, theme filtering, linked-face navigation or durable use tracking.
See the [Classic flow](CLASSIC-FLOW.md) and [trial contract](V3-TRIAL.md).

## 5. Coaching mode: current behavior

Enter through **compass → Options → Coaching**. Classic remains the boot default.
First entry silently selects an opening question in the model; later entries retain it,
but both enter the decorative compass rather than showing the opener card.
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

- Opening enters the wordless gold/blue-arrow compass with six labelled path icons.
  Its invisible **136x64 center** opens Options; independent decorative LCG wandering
  never chooses a path or consumes a draw. Options Back restores compass versus card.
- Options offers **Another opener**, **Guide me**, **Release**, **Classic**,
  **Instant**, **Reminders** and **Sound OFF/ON**, with Back to the previous context.
- **Another opener** in Options requests a new random opener by touch. A motion
  request works **only while on Opening**; path browsing cannot be replaced by it.
- A path restores its last question/experiment face. Question faces offer **Paths**,
  **Guide me**, **Release**, **Another** and **Try (optional)**; experiment faces offer
  **Question** to switch back without consuming a fresh opener.
- The path picker permits free path changes, return to **Opening**, or **Next step**.
  Release/Next step opens an optional closing without a quota or required experiment.
- Guide me adds private guidance for the current entry; Reminders are private cues,
  not automatic notifications or learner assessments.
- Opening from paths returns to the compass with the exact remembered opener retained.
  Explicit Another opener, a qualified Opening flip or sustained Opening shake switches to the blue opener card.
  Path question/experiment selection,
  last face and scroll context are kept independently in RAM.

No answer determines a path automatically. The mentor controls every question,
experiment and release, and can adapt, skip or stop. See [COACHING-FLOW.md](COACHING-FLOW.md).

## 6. Presentation, input and accessibility

The explicit new-opener card (not the compass entry) retains the approved narrower
coaching opening face of **192x112 pixels**, inside the unchanged
**256x140 snapshot**. Coaching body text is **18px**, opening wrap is **168px**,
and perimeter/card-variant Options captions are **12px**. Other navigation captions
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

Button fills remain `#0c1721` normal and `#1a2b38` pressed. Apart from footer removal and conditional text cues, the accepted geometry, fonts, icons and touch areas are retained.

**Sound** is one shared, RAM-only preference in Classic Options and Coaching Options.
It defaults **OFF after every reboot**. Booting and toggling either way are silent.
When ON, an accepted new Classic card or explicitly requested new Coaching opener
can play one **240ms, 500Hz decaying settlement chime**, using a nominal PCM peak
of **1536**. It begins when that draw becomes readable: normally after the six-second
reveal, or immediately on same-card reveal completion, Instant text or safe fallback.
The additional five-second reading hold is unchanged. Initial Coaching entry is
silent even though it selects its first opener; context restoration, Guide/Back,
path browsing, scrolling and ordinary navigation never cue. Busy cues are dropped,
not queued. OFF requests cancellation of active playback; switching ON again does
not revive that canceled cue. Audio uses a low-priority TX-only worker; the ready
main loop services it once after UI/IMU work. No RX, DIN/MCLK, microphone, storage,
networking or OTA is enabled by this feature.

Natural sustained shaking from the Coaching compass is physically confirmed to produce a card, with qualitative feedback that it is much better and consistent and that the current experience is liked. The UI cleanup was physically approved on v3-0.1.19 and retained exactly in v3-0.1.20 apart from release identity. No numerical trial count or separate Classic shake trial is inferred. Exhaustive readability/touch, Sound menu/mute/chime, sustained reliability, uptime and battery acceptance remain separate. **The original intermittent turnover-stuck-compass/blank issue remains OPEN**; the additional shake trigger is not a diagnosis or demonstrated fix.
Host geometry and rendered fit do not establish physical usability or reliability.

A deliberate sustained shake of roughly one second is an additional draw route, alongside turnover and touch; it goes straight to the existing reveal without requiring turnover. It is eligible only on Classic Home or a main card (Home draws Ask; a card retains active Ask/Try), or Coaching Opening (compass or opener card). Busy reveal, pointer contact, scrolling, Options, guidance, paths/questions/experiments and closings suppress motion. One sustained burst draws once; continuing to rattle cannot queue another card. Recognition requires 400ms of fresh quiet before intent, 900ms of credited fresh motion and ten opposing peaks, then a 2200ms cooldown followed by fresh quiet to rearm. These are implementation safeguards, not a prescribed handling ritual. The v3-0.1.20 correction replaces only the prototype 180 dps shake rotation veto with the existing 2044 dps gyro safety bound, retaining finite/per-axis clipping, age/gap and false-positive/rearm checks. Sensor configuration/driver and turnover recognition are unchanged.

Classic no longer shows the constant ASK/DRAFT/flip footer; canonical review states remain draft. Classic **More text** and the Coaching scroll hint appear only when actual laid-out text remains below the visible viewport, including the opener’s 14px bottom clipping mask—not merely because padding permits scrolling. Mode centers, Options targets, six path controls, content, typography and reveal/hold timing are otherwise retained.

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
4. Turnover/shake eligibility and shared `WindowReveal` → pinned LVGL 8.3.10 and board drivers.

Mode, pool, opener, path and scroll state are **RAM-only**. Returning to Classic and
re-entering Coaching within a boot preserves opener context; reboot persistence is not promised.
Stored draft text runs offline. There is no live AI, learner audio, conversation capture,
scoring, saving, use recording, team roster, Team Trail/history, cloud sync or V3 OTA.
The default-OFF bounded technical motion recorder is diagnostic RAM, not learner history.
Keep private diagnostics, credentials, device identifiers and runtime data out of public docs/builds.

## 8. Evidence, remaining gates and roadmap

The compiled S3 application is **691248 bytes**. Recorded v3-0.1.20 evidence includes 14 actual-LVGL native integration groups, 42 retained detector assertions and 26 new rotation-policy assertions, strict compilation and ASan/UBSan checks, plus independent scoped review. An exact private 5025-row replay of the failed v3-0.1.19 motion changes from zero events to exactly one with the single-rule correction; replay is not a new physical trial. Actual S3 compilation, application-only flash comparison and exact-version initialized-UI/real-IMU runtime verification passed. Those deployment and physical-trial results are retained release evidence; preparing this publication does not change the device. The final publication integration also passes the full canonical native suite and all 86 isolated Python checks without skips; 533 actual native-rendered frames were archived and hash-verified. The earlier 84-check/512-frame result remains historical v3-0.1.18 evidence.
Installation and running-version verification are deployment gates, not documentation tests.
Earlier natural Classic turnover observations and the current qualitative Coaching shake report are distinct acceptance evidence.

Team Trail, durable history/saves, filters, production eligibility, broader Home designs
and richer integrations are **unimplemented roadmap**, not prerequisites for mentoring
or capabilities of this installed trial. Their stable requirements and future acceptance
remain in [PRODUCT-SPEC.md](PRODUCT-SPEC.md); this baseline adds no feature commitments.
