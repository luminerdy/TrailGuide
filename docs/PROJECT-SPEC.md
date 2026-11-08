# TrailGuide — current project and system specification

## Installed27 / published-source checkpoint

**The development device runs `v3-0.1.27-accel16` DRAFT.** Full application readback and exact-version runtime checks passed. Shake now works in reported Classic compass/card and Coaching card contexts, but Coaching compass shaking fails and automatic shaking is intermittent after path selection; S then shake works there. Physical acceptance is partial, not a reliability or exactly-once result.
See the [current progress and verification ledger](V3-PROGRESS.md) for the installed scope, prior physical failures and remaining checks.

The committed firmware baseline remains `v3-0.1.22-reliability`. This documentation checkpoint does not integrate or publish the later private source snapshots or firmware packages. Earlier release descriptions below retain their historical scope; they are not current27 physical acceptance.

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

**v3-0.1.21-global-gestures DRAFT is installed**. This remains an offline authored-content trial, not a production release; local installation does not imply publication.
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
  A completed gesture from Home, Card, Guide or Options retains that active mode;
  only fresh boot defaults to Ask. Options closes before the new card appears.
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
  request works from every owned screen and interrupts any reveal/hold: it leaves
  the path, clears route history and draws a fresh Opening card.
- A path restores its last question/experiment face. Question faces offer **Paths**,
  **Guide me**, **Release**, **Another** and **Try (optional)**; experiment faces offer
  **Question** to switch back without consuming a fresh opener.
- The path picker permits free path changes, return to **Opening**, or **Next step**.
  Release/Next step opens an optional closing without a quota or required experiment.
- Guide me adds private guidance for the current entry; Reminders are private cues,
  not automatic notifications or learner assessments.
- Opening from paths returns to the compass with the exact remembered opener retained.
  Explicit Another opener, or a completed intentional flip/shake from any owned screen, switches to a fresh blue Opening card.
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

Qualified inversion blanks the previous owned scene without drawing; completed
return draws once, and cancellation restores the previous scene. A completed intentional
gesture interrupts reveal/hold instead of queuing a later draw. Only actual pointer
contact and inertial scrolling gate UI motion eligibility; menus and guidance do not.

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

The installed global start-fresh change is **qualitatively user-accepted in both Classic and Coaching**. This does not establish a trial count, exhaustive per-screen/per-gesture coverage or sustained field reliability. Earlier Coaching-compass shake and UI-cleanup acceptance remain historical evidence. Exhaustive readability/touch, Sound menu/mute/chime, uptime and battery acceptance remain separate. **The original intermittent turnover-stuck-compass/blank root cause remains OPEN**; global policy acceptance is not a diagnosis or proof of its repair.
Host geometry and rendered fit do not establish physical usability or reliability.

A completed intentional turn over and back or a deliberate sustained shake starts fresh from **any owned screen**, including Options, Guide, paths, questions, experiments, the path picker, closings, reminders, the Opening compass and an active reveal or reading hold. Classic draws once in its active Ask/Try mode, including after Home, and clears Options before showing the card. Coaching leaves the current path, clears route history and draws one fresh **Opening card**, not another question in that path or a return to the compass. Old reveal/hold resources and pending settlement cues are canceled: no old callbacks, queued draws or stale cues reach the replacement. Actual pointer contact and inertial scrolling still suppress motion. Partial gesture intent and qualified blackout survive autonomous same-card reveal settlement and hold deadlines. Turnover stays black while qualified inverted and starts the new reveal on completed return; shake goes straight to reveal without an extra timed black pause. Each new card retains the six-second whole-face reveal plus five-second reading hold. One sustained burst draws once; continuing to rattle cannot queue another card. Recognition retains 400ms of fresh quiet, 900ms of credited fresh motion and ten opposing peaks, then a 2200ms cooldown followed by fresh quiet to rearm. These are safeguards, not a prescribed handling ritual. The v3-0.1.20 correction replaces only the prototype 180 dps shake rotation veto with the existing 2044 dps gyro safety bound, retaining finite/per-axis clipping, age/gap and false-positive/rearm checks. Sensor configuration/driver and turnover recognition are unchanged.

Classic no longer shows the constant ASK/DRAFT/flip footer; canonical review states remain draft. Classic **More text** and the Coaching scroll hint appear only when actual laid-out text remains below the visible viewport, including the opener’s 14px bottom clipping mask—not merely because padding permits scrolling. Mode centers, Options targets, six path controls, content, typography and reveal/hold timing are otherwise retained.


## Global gesture state and contact gates

| Owner / current state | Completed intentional flip or rearmed shake | Gates / restoration |
| --- | --- | --- |
| Classic Home / compass | One Card in active Ask/Try; fresh boot starts Ask, Home does not reset Try. | Live owned root, no pointer contact or inertial scrolling. |
| Classic Card / Guide / Options | One Card in active Ask/Try; clear Options before rendering Card. | Same gates; ordinary Guide/Back still restores the same card. |
| Classic reveal / reading hold | Cancel old resources/cue and start one fresh Card in active mode. | Same gates; autonomous settlement preserves partial intent/blackout. |
| Coaching Opening compass / opener card | One fresh Opening card, not compass entry. | Same gates; normal Options Back preserves compass/card variant. |
| Coaching Question / Experiment / Guide / Reminders / Closing / Picker / Options | Leave current path, clear route history and draw one fresh Opening card. | Same gates; ordinary Back, Opening and path restoration are unchanged. |
| Coaching reveal / reading hold | Cancel old resources/cue and start one fresh Opening card. | Same gates; no delayed old draw, callback or cue. |
| Either owner with actual pointer contact / inertial scrolling | No gesture draw. | Cancel partial intent; require existing recognizer rearming. |
| Missing owned root or unavailable / invalid sensor input | No unqualified draw. | Existing freshness, validity, health, clipping and quiet/cooldown handling apply. |

Options and reveal flags remain descriptive diagnostics; they are not by themselves
eligibility blocks. All detector thresholds, IMU driver/configuration, models,
content and whole-face renderer are unchanged from v3-0.1.20. Qualified inversion
stays black across autonomous reveal/hold deadlines until completion or cancellation;
a shake adds no timed black pause. Ordinary touch **Opening**, **Back**, Options
return and saved path/face/scroll restoration do not become fresh draws.

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

The installed **v3-0.1.21-global-gestures** S3 application is **691312 bytes**. Release evidence includes 12 global-policy cases and 14 actual-LVGL integration groups under ASan/UBSan and warnings-as-errors, actual S3 compilation and independent scoped review. Application-only flash comparison, exact live Version, initialized active UI and advancing valid real-IMU readings passed. This documentation update does not contact hardware. Historical v3-0.1.20 evidence remains separate: 42 retained detector assertions, 26 rotation-policy assertions, 14 actual-LVGL integration groups, strict/sanitized checks and independent review; its canonical native suite, 86 isolated Python checks and 533 archived native frames are not new v3-0.1.21 results. The earlier 84-check/512-frame evidence belongs to v3-0.1.18.
Installation and running-version verification are deployment gates, not documentation tests.
Earlier natural Classic turnover observations and the current qualitative Coaching shake report are distinct acceptance evidence.

Team Trail, durable history/saves, filters, production eligibility, broader Home designs
and richer integrations are **unimplemented roadmap**, not prerequisites for mentoring
or capabilities of this installed trial. Their stable requirements and future acceptance
remain in [PRODUCT-SPEC.md](PRODUCT-SPEC.md); this baseline adds no feature commitments.
