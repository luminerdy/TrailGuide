# V3 — round TrailGuide companion specification

## Installed26 / published-source checkpoint

**The development device runs `v3-0.1.26-manual-s` DRAFT.** Full application readback and exact-version runtime checks passed; physical S-assisted shake acceptance is pending. Automatic shake reliability remains open.
See the [current progress and verification ledger](V3-PROGRESS.md) for the installed scope, prior physical failures and remaining checks.

The committed firmware baseline remains `v3-0.1.22-reliability`. This documentation checkpoint does not integrate or publish the later private source snapshots or firmware packages. Earlier release descriptions below retain their historical scope; they are not current26 physical acceptance.

## Scope and authority

This is the hardware/runtime contract for the round ESP32-S3 target. The [project specification](PROJECT-SPEC.md) defines the current system baseline; [PRODUCT-SPEC.md](PRODUCT-SPEC.md) retains stable product requirements, including P-19 mentoring, P-20 playful reveal and P-21 Classic/Coaching. [CLASSIC-FLOW.md](CLASSIC-FLOW.md) and [COACHING-FLOW.md](COACHING-FLOW.md) map the actual mode flows.

V3 is an optional enjoyable companion to project-led mentoring, not the curriculum or a prerequisite for learner independence. V1 OLED and V2 portrait C6 remain separate targets with preserved fallbacks. Do not reuse their drivers, pins, partitions or binaries.

## Current baseline and evidence

**Approved direction: Coaching-first, with revised V3 wording as its content foundation.** This is not a startup-default change or a content merge: Classic still boots by default and remains an intact optional quick-prompt route. Coaching currently uses its own authored layer plus canonical references; neither all twelve Classic pairs nor the whole forty-card catalog has been merged into Coaching.

**v3-0.1.21-global-gestures DRAFT is installed**. This remains an offline authored-content trial, not a production release; local installation does not imply publication.

The installed **v3-0.1.21-global-gestures** S3 application is **691312 bytes**. Release evidence includes 12 global-policy cases and 14 actual-LVGL integration groups under ASan/UBSan and warnings-as-errors, actual S3 compilation and independent scoped review. Application-only flash comparison, exact live Version, initialized active UI and advancing valid real-IMU readings passed. This documentation update does not contact hardware. Historical v3-0.1.20 evidence remains separate: 42 retained detector assertions, 26 rotation-policy assertions, 14 actual-LVGL integration groups, strict/sanitized checks and independent review; its canonical native suite, 86 isolated Python checks and 533 archived native frames are not new v3-0.1.21 results. The earlier 84-check/512-frame evidence belongs to v3-0.1.18.

The installed global start-fresh change is **qualitatively user-accepted in both Classic and Coaching**. This does not establish a trial count, exhaustive per-screen/per-gesture coverage or sustained field reliability. Earlier Coaching-compass shake and UI-cleanup acceptance remain historical evidence. Exhaustive readability/touch, Sound menu/mute/chime, uptime and battery acceptance remain separate. **The original intermittent turnover-stuck-compass/blank root cause remains OPEN**; global policy acceptance is not a diagnosis or proof of its repair.

See [V3-TRIAL.md](V3-TRIAL.md) and [HANDOFF.md](HANDOFF.md).

## Target hardware

- Original **Waveshare ESP32-S3-Touch-LCD-1.85 Rev2.1**, not 1.85B or 1.85C-BOX.
- 360x360 round capacitive touchscreen; ESP32-S3; 16MB flash and nominal 8MB PSRAM.
- ST77916 display, CST816-family touch, TCA9554 support and QMI8658 accelerometer/gyroscope use the inspected original-family package.
- No magnetometer is required. The wandering compass is decorative and must not imply real north, calibration or navigation accuracy.
- USB power is the baseline. Battery/enclosure selection, connector/polarity/charging checks and measured runtime are deferred.

Use the recorded board identity and pinned pin map. Re-identify the actual target, revision and live port before any operation that can reset/write it; USB enumeration alone does not verify the display or fitted peripherals. Preserve the two original full-flash recovery images and accepted firmware artifacts privately.

## Modes and navigation

### Classic — prompt deck

- Boot to an uncluttered full-face compass with no automatic draw; Ask is the fresh-boot default.
- A compass tap opens Options: **Ask the Team**, **Try a Move**, **Coaching**, **Instant text**, **Sound OFF/ON** and **Back**.
- Ask/Try touch actions draw immediately in that mode. A completed intentional turn over and back or sustained shake from any owned view retains active Ask/Try, including Home, Guide, Options and reveal/hold; Options clears before Card. Home returns to the compass without drawing.
- Twelve selected card identities have independent shuffled Ask/Try bags, no within-cycle repeats and no immediate reshuffle-boundary repeat.
- **Guide me** opens listening/activity support for the selected identity; Back returns without drawing. **Another** draws in the active mode. **More text** and scrolling retain the current copy. Home returns to the compass.

### Coaching — optional conversation aid

- Enter explicitly from Classic Options to the wordless full gold/blue-arrow compass, retaining the silently selected/saved opener in RAM. Six labelled path icons remain selectable. The invisible 136x64 center opens Options; Back restores compass versus opener card. Independent decorative LCG wandering never selects a path or draws content. Coaching **Options → Classic** returns to the original compass.
- Six contexts appear at 12/2/4/6/8/10 o'clock: **Progress, Stuck, Unclear, Exploring, AI Results, Team**. Compact captions shorten Exploring to Explore and AI Results to AI.
- Each path has four questions and three optional experiments. Another cycles in authored order; switching Question/Experiment or paths preserves independent face/position/scroll state.
- Guide me and listening reminders are private facilitator aids. Release/Next step leads to optional closing questions without requiring a path, a question quota or an experiment.
- Opening from paths returns to the compass, preserving the exact saved opener. Another opener in Options, or a completed intentional turnover/shake from any owned screen, leaves the current path, clears route history and draws a fresh blue Opening card through the retained reveal/hold.
- Coaching Options includes **Sound OFF/ON**, sharing the Classic preference.
- Nothing automatically listens, selects a context, advances the conversation or evaluates a team. Context is RAM-only and resets on reboot.

## Motion and presentation contract

The turnover route retains this sequence; the additional sustained shake selects once and goes straight to the same reveal without requiring inversion.

1. A fresh eligible held inversion blacks the existing compass/card without selecting a replacement.
2. A completed qualified return selects exactly one new card/opener and begins the retained whole-answer-face reveal.
3. Cancellation, timeout or invalid context/health restores the prior scene without drawing.
4. The entire blue answer-bearing surface and complete text emerge together with depth/tilt/drift over **6000ms**; do not substitute typing or a static-label fade.
5. An additional **5000ms** card-only reading hold follows settlement, then controls return. A tap can finish the same reveal/start its hold; a subsequent settled tap can expose controls.
6. A completed intentional turnover/shake can interrupt either reveal or settled hold. Cancel old owned resources and pending cues before the fresh draw; never queue it or let stale callbacks affect its replacement. Preserve partial intent and blackout across autonomous same-card settlement.
7. Touch routes and Instant text keep motion/animation optional. Safe rendering fallback must retain complete copy and controls; it is not successful animation.

All owned Classic and Coaching views are motion-eligible, including Options, Guide, paths and reveal/hold. Actual pointer contact and inertial scrolling suppress motion; sensor health/freshness and recognizer rearm gates still apply. Gesture thresholds, rest/return timing and cancellation behavior must not be retuned for a cosmetic change.

The owned driver retains the supported ±2048dps gyro range, matched 16LSB-per-dps conversion and safety bounds. Keep register selection, conversion and clipping predicates coupled. Technical capture scaling belongs to its exact firmware; do not reinterpret older recordings using newer settings.

A completed intentional turn over and back or a deliberate sustained shake starts fresh from **any owned screen**, including Options, Guide, paths, questions, experiments, the path picker, closings, reminders, the Opening compass and an active reveal or reading hold. Classic draws once in its active Ask/Try mode, including after Home, and clears Options before showing the card. Coaching leaves the current path, clears route history and draws one fresh **Opening card**, not another question in that path or a return to the compass. Old reveal/hold resources and pending settlement cues are canceled: no old callbacks, queued draws or stale cues reach the replacement. Actual pointer contact and inertial scrolling still suppress motion. Partial gesture intent and qualified blackout survive autonomous same-card reveal settlement and hold deadlines. Turnover stays black while qualified inverted and starts the new reveal on completed return; shake goes straight to reveal without an extra timed black pause. Each new card retains the six-second whole-face reveal plus five-second reading hold. One sustained burst draws once; continuing to rattle cannot queue another card. Recognition retains 400ms of fresh quiet, 900ms of credited fresh motion and ten opposing peaks, then a 2200ms cooldown followed by fresh quiet to rearm. These are safeguards, not a prescribed handling ritual. The v3-0.1.20 correction replaces only the prototype 180 dps shake rotation veto with the existing 2044 dps gyro safety bound, retaining finite/per-axis clipping, age/gap and false-positive/rearm checks. Sensor configuration/driver and turnover recognition are unchanged.

Classic no longer shows the constant ASK/DRAFT/flip footer; canonical review states remain draft. Classic **More text** and the Coaching scroll hint appear only when actual laid-out text remains below the visible viewport, including the opener’s 14px bottom clipping mask—not merely because padding permits scrolling. Mode centers, Options targets, six path controls, content, typography and reveal/hold timing are otherwise retained.

## Layout and input contract

- Buttons retain `#0c1721` normal / `#1a2b38` pressed. Apart from the footer/conditional-cue cleanup, accepted fonts, icons, geometry and touch areas are retained.
- Classic retains its existing compass/card layout; do not apply Coaching's geometry to it.
- Explicit new-opener draws (not compass entry) retain a **192x112** blue face inside the exact **256x140** snapshot panel, centered by trimming 8px from each previously approved side.
- Opening copy wraps at **168px**, with **18px** main text and **12px** perimeter/opening Options captions. Text coordinates/wrapping and icon positions are unchanged by the side trim.
- The compass center has an invisible **136x64** Options target. The legacy opener-card variant retains its plain Options text and **100x48** target. Path targets remain at least 48px; visible caption size is not the hit-area size.
- All six canonical openers fit in native rendering. Longer path/guidance/overflow copy remains genuinely scrollable, with no clipped essential text or accidental button activation during drag.
- Preserve successful non-fallback capture, intermediate reveal pixels and exact settled-raster/live-text handoff. A smaller visible face must not shrink the renderer's required snapshot.
- All controls/text/focus feedback must be usable on the actual circular panel, with real pointer and physical checks separate from geometry tests.

## Optional settlement sound

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

## Software architecture and dependencies

- Authored source: [cards.json](../content/cards.json) and [coaching.json](../content/coaching.json).
- Validated generators produce separate Classic and Coaching headers; canonical identities/review states are preserved.
- Pure models own bag/navigation/context behavior; UI owns LVGL objects, touch and timer lifetimes. Sensor sampling, Gesture recognition and WindowReveal remain distinct.
- Pinned build baseline: Espressif Arduino core **3.3.2**, original-family vendor drivers and target-bundled **LVGL 8.3.10** with the reviewed display DMA adapter. Use the intended font configuration for both C and C++.
- Cancel owned timers and release raster resources on navigation, object deletion and destruction. No per-animation-frame heap allocation or history write is permitted.
- Keep the finite positive HWCDC transmit budget that prevents unread-monitor stalls; diagnostics must not block normal UI progress.

## Flash, recovery and diagnostics

- Retain the compatible existing partition table and bootloader. The application budget is `0x300000`; accepted app slots start at `0x10000` and `0x310000`. Their existence does not mean V3 OTA is implemented.
- Authorized USB deployment is application-only, preceded by original-board, source, image/slot, security, recovery and evidence-preservation checks, then explicit flash comparison and fresh exact-version runtime readback.
- Before preparation or regression runs, create a new isolated output increment and protect every installed release root/descendant. Never rerun the current default preparer over an accepted installed artifact tree.
- The default-OFF technical RAM recorder is bounded to 8192 rows / 120 seconds, with session/token/range provenance and complete CRC-checked export. It records technical sensor/gesture evidence, not conversation content or learner outcomes.
- Preserve any valuable active/frozen recording before reset/write. USB open and JTAG inspection can perturb execution; exclude those intervals from natural-motion claims.

## Verification and acceptance

For the current read-only trial:

- Validate source/generation, all selected content, zero/single/max pools, bag boundaries and navigation/context/RNG invariants.
- Exercise actual bundled LVGL with ASan/UBSan: complete copy, real pointer/drag routes, all six paths, unrestricted release, overflow, allocation/deletion/cancellation, blackout, reveal and handoff.
- Compile the intended S3 target; check image identity, slot budget and unchanged compatible components. Verify the exact written target and resumed UI/IMU liveness.
- Separately collect physical Sound menu/mute/chime, readability, edge/touch/scroll, deliberate turnover/shake and ordinary-handling observations. The current Coaching shake report is qualitative, not exhaustive motion acceptance. Do not invent a shake-test quota or require extra gestures to accept a cosmetic change.
- Sustained operation, reboot behavior and independent power need their own observed evidence. Battery readiness requires measured charging/runtime checks. No production or educational-effectiveness claim follows from a software gate.

## Explicit future scope, not installed functionality

V3 currently has **no** team roster, theme-filter UI, durable Team Trail, saved/history/feedback/Undo, participant data capture, idle/wake persistence, live AI, microphone/input audio, cloud sync or OTA service. V2's authenticated LAN updater is separate and is not evidence of V3 networking.

The broader future production contract in PRODUCT-SPEC.md includes filtering and deliberate, private, durable team records with attribution/deduplication/migration gates. Those features require their own approved implementation and tests; they are not prerequisites for mentoring or the current read-only Classic/Coaching trial. Optional enhancements must not silently become dependencies of the working device.

Browser studies remain separate simulations, not firmware, a board emulator or physical acceptance. Source/docs may be public; original recovery images, credential-bearing builds, private logs and captures stay private.

[Manufacturer reference](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85)
