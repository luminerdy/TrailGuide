# TrailGuide — project and coaching handoff

## BLUF

TrailGuide's main effort is **project-led mentoring across varied AI uses**, for individuals and teams, and helping learners continue without depending on the mentor. Start with something meaningful to make, investigate or improve; use the next useful piece to focus learning. Research and building can be learning activities even when the artifact contains no AI. The physical device remains an optional, enjoyable companion and building experiment—not the curriculum or a constraint on all materials.

Draft mentor practice and a learner-owned project/coaching brief connect to **AI Learning Lab's Define → Direct → Check → Adjust** and flexible AI roles. The next primary step is to use/review them on one real project milestone, inspect usefulness and carry context into a next conversation. They are not a validated curriculum or an integrated AI-coaching service.

The earlier, separate V3 browser experience study explores **shake → one random curated idea → gradual text reveal**, with simulated shake, touch fallback, Reveal all and Instant text. It is not the native interaction: current V3 uses simple turn over and back, now confirmed for one recorded natural cycle; see the V3 checkpoint below. Current V2 is **v2-0.1.8**, adding touch-triggered gradual main-card reveal and same-card Reveal all, with instant Guide/Back. It was installed over authenticated local Wi-Fi; running version, changed application slot and continuing OTA readiness were read back. Physical pacing/readability/touch review remains open.

This handoff records a checkpoint, not a guarantee of live hardware availability. Inspect the current Git commit/status for the exact source revision. V2's reveal is a presentation-only firmware increment; V1, canonical card copy/IDs/revisions/review states, model/deck order, networking and partitions remain unchanged. No USB upload or serial reset was used for the reveal update.

## Read in this order

1. [README — vision versus implementation](../README.md).
2. [Project-led mentoring direction and examples](COACHING-DIRECTION.md), then the [portable learner-owned project/coaching brief](PROJECT-COACH.md).
3. [Canonical product requirements](PRODUCT-SPEC.md), especially P-19 purpose and optional P-20 experience, alongside device P-16/P-18.
4. [Delivery plan](PLAN.md) and [actual verification record](BRINGUP.md).
5. [Two-mode content/navigation contract](FACILITATOR-MODES.md).
6. [V2 trial build/review](V2-TRIAL.md), [V2 Wi-Fi/OTA](V2-WIFI-UPDATES.md) and [original-image recovery](V2-BRINGUP.md).
7. [V3 initial chip/backup/build evidence](V3-BRINGUP.md) and [experience study boundaries](V3-EXPERIENCE.md), separate from the retained full-flow round preview.
8. Repository `AGENTS.md` before taking development actions. Its contributor instructions remain unchanged in this increment; any protected-file alignment requires separate approval.

The older [Pi handoff](PI-HANDOFF.md) is V1 host-setup guidance, not the current project status.

## 1. Direction to preserve

Use the learner's project and **Define → Direct → Check → Adjust** as the working context. ASK / COLLABORATE / ORIENT / TEACH / ENABLE / DELEGATE / ORCHESTRATE are flexible choices, not a mandatory ladder or assessment. **Notice → Invite → Listen → Offer a small next step → Reflect** remains a proposed mentor listening scaffold within that work, not a competing curriculum or proven protocol.

- Make room for uncertainty without putting someone on the spot.
- Ask about what a learner tried and noticed before assuming a personal problem.
- Offer a hint, example, thinking time, a small test or a different participation format; passing and declining AI use remain legitimate choices.
- Evaluate a particular AI contribution in its task/risk context. Do not label people as over-trusters, skeptics or weak learners.
- Cover creation, research, learning, analysis, coding, planning and collaboration. Trust/verification belongs throughout, not as the whole curriculum.
- Support the mentor before, during and after project work; offer a learner-owned context/help-style brief for continuing with AI or human help. Independence does not mean never requesting support.
- Begin practice outside the live device rather than adding another home mode. Hardware/storage acceptance is not a prerequisite for using the draft materials.
- Preserve privacy and agency: no hidden microphones/recording, profiles, diagnoses, learner scoring or inferred engagement/ability.

See COACHING-DIRECTION.md for synthetic hesitant-participant, convincing-AI-answer, skeptical-participant and blocked-task examples. These are drafts for research/editorial review, not newly released card content.

## 2. What exists versus what does not

### V1 — working fallback

Classic ESP32, SH1106 128x64 OLED and BOOT input, firmware v1-0.2.4 with twelve original cards. Sampled readability and mechanical button operation were human-confirmed. Remaining soak/full-deck/independent-power acceptance is recorded in BRINGUP.md. Preserve its firmware, private configuration and recovery images.

### V2 — preserved portrait trial

ESP32-C6 1.47-inch, nominal 172x320 portrait capacitive touch, pin-header family. C6 silicon revision 0.2 and 8MB flash were probed; exact physical PCB/variant confirmation remains open. Manufacturer-demo operation was human-reported working. esptool package labels differ by version; do not substitute either label for physical identification.

Running checkpoint: v2-0.1.8 DRAFT, installed over Wi-Fi and remotely version/OTA-readiness/changed-slot verified.

The rejected v2-0.1.7 uniform text fade is replaced by the approved six-second physical-window direction: the complete blue answer-bearing surface and words approach, gently tilt/drift and settle inside a dark window. The native v2-0.1.8 implementation is now installed and remotely boot/OTA-verified. It approximates the browser study with nearest-sampled RGB565 perspective, without browser blur/soft shadows; physical motion, allocation success and frame rate remain to be observed. A bounded 106,240-byte caller buffer is outside the unchanged 48KiB LVGL heap; allocation failure retains instant complete scrollable text, not working motion.

Implemented trial:

- Home: Ask the Team and Try a Move, no automatic draw on fresh boot.
- Eight canonical draft pairs, one per theme; fixed review-order Another rather than production shuffled/filterable pools.
- One Guide me entry; Back preserves the card. Complete copy is vertically scrollable.
- New Ask/Try/Another draws animate the whole native blue face/words together for 6000ms including 600ms darkness, then settle onto the identical final scrollable raster. Fixed controls offer same-card Reveal all; Guide/Back remain instant. Navigation/rerender, OTA and independent object/owner deletion cancel timers and release image/cache storage. No new menu, main-text shortcut, sensor or tracking is added.
- Ask: What to listen for → direct cue → You could ask.
- Try: How to run it → What to notice → You could ask → optional If helpful, offer.
- Redundant leading Listen for / Look for / Notice is removed from displayed cue bodies only. Remaining wording, negations/cautions, mixed-case names and acronyms are retained.
- Optional station Wi-Fi, background reconnect, authenticated local OTA, updating input-blocking overlay and a minimal read-only status endpoint. Guidance remains usable offline.

Not implemented: theme picker, production eligibility/pools, linked-face shortcut, team roster, saving/recording, durable Team Trail, facilitator practice mode, live AI, learner monitoring or cloud updating. Reserved partitions or tests must not be mistaken for those features.

### V3 — qualified-turnover blackout installed and observed

Current **v3-0.1.11 DRAFT** is installed and exact-version/UI/bus/real-IMU verified. Plain full-face compass/tap-to-Options, twelve shuffled draft pairs, Guide/Back, whole six-second reveal and five-second settled card-only hold are unchanged. Compass/touch were positively reported; one natural turnover now showed blank → card animation → controls, with one clean recorded trigger. Repeated-use reliability and card-only timing remain separate acceptance checks. The approved twelve-card wording and optional guidance are installed; bubbles remain preview-only.

Actual v3-0.1.5 trace had 315 valid eligible resting rows stuck in Recover with gravity opposite the remembered origin. The former inversion veto blocked rearming indefinitely. v3-0.1.6 permits a new stable eligible reference after existing 1200ms rebase dwell (same-origin 600ms), retaining the old one until qualification. Reacquisition clears the canceled cycle/dwell and never draws. Active Ready/Turning/Inverted, thresholds, cooldown, sensor/model/content/renderer and positive USB TX budgets are unchanged.

Old synthetic/private replay fails; corrected actual UI/Gesture/LVGL rearms without drawing, then synthetic full cycles draw exactly once, with no busy queue. Actual LVGL 8.3.10 ASan/UBSan, pinned full SDK HWCDC regression and 38 isolated Python tests pass; independent review found no blockers. All 5,516 installed v1.5 files and inventory remain byte-identical. Current build 638,671 program/127,640 globals, exported app 638,816 bytes; exact target/hash/slot/current-prepared sources, originals, same-board identity and compatible partitions passed before app-only write/hash verification.

Startup/UI/actual IMU and healthy first heartbeat were read back. Large later heartbeat lines can truncate under bounded TX; do not fabricate complete records. With reader closed for ten seconds, direct read-only debug snapshots showed compass ticks 400→463 and fresh UI/sample timestamps 64623→74668ms, motion healthy/Ready and TX budget 1ms. CPUs resumed; monitors/debuggers closed. Current build/evidence: `build/v3-0.1.6/`; operating contract: [V3-TRIAL.md](V3-TRIAL.md).

Historical freeze: v1.4 diagnostics set HWCDC timeout 0, causing unsigned retry underflow in normal unread heartbeat output. Actual GDB stack proved it; v1.5+ sets a finite positive 1ms budget at setup/activation. Never restore zero. Raw proof: `build/v3-motion-investigation/freeze-observation/`. A USB disconnect/reconnect occurred during temporary recovery; its cause was not established. Broader timing/power/heap/PSRAM/handling/production storage and OTA remain separate. Local commits are not GitHub publication; authentication renewal is still pending.

The latest physical v3-0.1.6 sequences still produced no card. An unattended, explicitly synthetic return-state injection did exercise the actual target draw/reveal/hold/control-restoration path with real IMU samples; cleanup reset returned Home/Ready. This is not physical gesture acceptance. Both the bounded RAM recorder and approved twelve-card update are now installed in v3-0.1.8 after parent package/write/readback checks; the physical flip remains unresolved. Evidence: `build/v3-motion-investigation/target-draw-bench/`.

The v3-0.1.7 opt-in flight recorder now retains actual poll attempts in bounded PSRAM instead of relying on a live USB stream. Actual target verification: RECORD acknowledged a fresh session, reader closed 46seconds, then all 1,969 records / 78,760 bytes were fetched and independently CRC-verified without reset. Every baseline row was valid, eligible and Ready; zero clipping/read errors/resets, actual poll gaps 20–36ms. This was an unattended baseline, not physical flip evidence. Frames include raw rails/STATUS/wait/error, UI reset causes and triggers, so a later human movement can be diagnosed without chat timing. Strict unsigned command parsing was fixed RED/GREEN after independent review; full ASan/UBSan/SDK and 42 isolated Python checks passed, followed by independent unblock review. Current program 640,967/global128,176; exported app 641,120 bytes. Current package `build/v3-0.1.7/`; client `tools/v3_flight_recorder.py`.

The twelve-card content was recovered from the endorsed draft and independently reviewed: exact openings/private cues/conditional follow-up/optional help, 12 revisions advanced once, 28 other records unchanged, all review scopes draft. V3 model count derives from generated 12; V2/browser selectors remain 8. Four clean harness/preparer merges retained recorder and typography tests. Actual merged 45 Python checks, LVGL 48 contexts/179 pages, expanded bags/legacy RNG, SDK/recorder/differential tests and integration review passed. Build645,507 program / 128,184 globals; app 645,648 bytes. Exact installed 1.8 source/hash/slot/components/originals/same-board identity and startup/UI/IMU passed. A real-target explicitly synthetic return-state bench selected tg-004 and read its exact approved question from ROM; normal reveal/hold/restoration completed, and full 1,904-row/76,160-byte recorder export CRC f09b5d32 retained exactly one synthetic trigger. This is not human-motion success. Evidence exported before intentional cleanup reset; Home/index-1/compass verified and monitors closed. Current package `build/v3-0.1.8/`; [V3-CARD-CONTENT.md](V3-CARD-CONTENT.md) records editorial scope.

Latest V3 v3-0.1.11 adds only the approved phase-linked presentation: existing qualified inversion hides old compass/card to black without selecting, allocating or resetting; confirmed return calls the original draw/reveal once. No new shake, extra delay or change to 2048dps recognition, timing, model/RNG, cards, renderer or recorder. Real pointer cancellation restores/swallow-clicks, focus deadlines retain hidden chrome masks, and lifecycle/timeout/health routes restore safely. Bounded mask supports normal≤6 children; foreign>32 roots decline blackout, and foreign reordering is unsupported.

All 53 isolated Python and full native LVGL/content/SDK/differential checks and independent 14-file review pass. Native 13-frame preview has exact all-black inversion/preconfirm frames and matching cancellation-restoration bytes. Actual target 646,191 program/128,192 globals, app 646,336 bytes passed package/original/same-board/security/source/compatible components and app-only write/explicit verify. Exact 1.11 runtime/UI/IMU/compass and default-OFF recorder verified. Legacy streaming probe failed to produce accepted rows; direct exact-ELF runtime checks passed and are the evidence, not that probe.

The human confirmed blank, card animation, then other buttons. Whole CRC: 4940 records/197600 bytes over 112.156seconds retain exactly one natural trigger, zero clipping and no read errors. Post-freeze-only JTAG verified selected Card, blackout/reveal/focus cleared. No synthetic input during movement/animation and no reset afterward. Physical visual sequence accepted once; repetition/physical cancel paths remain separate from native tests. Current package `build/v3-0.1.11/`; working 1.10 fallback and its reported Card/Compass successes remain preserved.

Earlier V3 v3-0.1.10 corrects the captured saturation with one checked range selection: CTRL3 0x77, +/-2048 dps/16 LSB per dps, safety 2044 dps with unchanged raw headroom. Both safety predicates and the conversion match; no new gesture, timings, rest threshold, gates, card/content/renderer or recorder changes. All 53 isolated Python and full native/content/SDK checks pass; 12000 equal-raw plus 12000 independently encoded equal-physical differential steps and independent 18-file review pass. Actual S3 image/source/backup/same-board/security/partition gates and app-only write/explicit verification pass. Exact 1.10 ROM/UI/IMU config/readback and live compass/sample counters verified.

One fresh natural turn-over-and-back now produced a card, human-confirmed, with exactly one recorded trigger: 3610 rows/144400 bytes/83.068seconds/fullCRC, zero clipping and no read errors. Turning/Inverted progression is present. No debugger or synthetic state during movement; JTAG read only after freeze. Firmware/range provenance is retained with raw data; older 512dps captures are not reinterpreted. Leave the running card/device undisturbed; repeatability remains open and serial long-dump reliability is a separate limitation. V1/V2 remain untouched. Current package `build/v3-0.1.10/`; [V3-TRIAL.md](V3-TRIAL.md) gives the precise scope.

The diagnostic-only v3-0.1.9 checkpoint extends REC to 120seconds/8192x40-byte rows (327,680 PSRAM bytes), adds strict-session idempotent FREEZE, and retains old 2304-capacity imports in the updated client. Legacy MOTION remains 45seconds. All 53 isolated Python checks, actual LVGL/content/SDK/driver-differential suites and independent review pass; unsigned metadata validation was tightened RED/GREEN before publication. Actual S3 build 645,639 program bytes/128,184 globals; app 645,792 bytes. Fresh original-board/security/16MB/backup/image/component/source checks, app-only write and explicit flash verification passed. Exact 1.9 ROM version, Home, healthy IMU and advancing compass/sample counters read back.

Actual target synthetic return testing confirmed 327,680-byte recorder allocation concurrently with 143,360-byte reveal storage, then normal reveal/hold/control restoration. Manual FREEZE reason 3 retained 3725 rows/149000 bytes with exactly one synthetic trigger. Long serial exports showed timeouts/interleaved heartbeat text; accepted complete retrieval used read-only JTAG of already-frozen RAM plus full CRC, not partial serial data. Evidence exported before intentional cleanup reset to Home/defaultOFF; this is not human flip or visible-pixel acceptance.

A subsequent complete natural turnover recording retained 3101 rows/124040 bytes over 70.862seconds, manually frozen with 70ms host ACK round trip. JTAG was used only after freeze, so it did not perturb the movement. The sensor observed opposed orientations, but gyro-X hit both raw rails on 8 polls; invalid sample health cancelled the cycle into Recover during both legs. No new bus read errors or touch gates explain those cancellations. Human result remained compass/no card. Next evidence-based candidate is a supported wider gyro measurement range with matching units/safety checks, NOT a more complicated gesture or arbitrary threshold relaxation. The clipped true peaks cannot be reconstructed from this trace. Private evidence and exact source checkpoints remain local; raw data is not public.

### Content

`content/cards.json` is canonical: forty draft cards across eight themes, five per theme. Twelve existing cards have paired draft guidance and form the V3 selection; the other twenty-eight do not. Ten existing cards also have optional CLI-agent adaptations; they are not extra cards or another theme. Structural tests are not editorial approval.

## 3. Guide-writing agreement

Keep the main question/action distinct from the help. A heading already supplies its instruction: under What to listen for, begin with “Alternatives that change the approach…” rather than “Listen for alternatives…”. Follow-ups are optional suggestions, not an interrogation or an automatically chosen branch.

The current formatter changes presentation, not canonical editorial text, IDs, revisions or review states. If authoring changes meaning/canonical copy, increment only affected revisions, preserve stable IDs and synchronize the catalog/tests. Never silently mark drafts reviewed.

Keep the live tool glanceable on 172x320. Preserve one help entry, readable copy, complete overflow access and touch safeguards. Do not solve wording problems with extra menus or claim browser/host rendering proves physical readability.

## 4. Research and training work to pick up

Separately collected research notes and a bounded synthesis are inputs for review, not a public curriculum or proof of these scripts. Keep private workspaces and participant material outside the repository. Check original citations, context/transfer limits and contradictions before making effectiveness claims; do not imply exhaustive current coverage or treat the proposed listening scaffold as validated.

Research questions:

- How can a learner's chosen project supply a useful next step across creation, research, analysis, building, learning and collaboration?
- Which mentoring moves help people choose assistance, inspect results and continue with a context brief rather than depend on a facilitator?
- Which coaching/learning principles support invitation, listening, feedback, useful effort and voluntary participation?
- How should a facilitator support both overreliance and reasonable skepticism without pushing AI adoption?
- What distinguishes better AI-assisted task performance from durable understanding and unaided transfer?
- Which findings come from trials, self-report studies, expert consensus or untested proposals, and where do they fail to transfer to adult workshops?
- What access needs, participation alternatives and power dynamics should shape the pilot?

Draft practice and a portable project/coaching brief now exist. Review them through one real project milestone and voluntary feedback; retain an honest human-pilot plan before making learning claims. Cited evidence should support a concrete project/mentoring decision, not become a separate abstract curriculum. Read original sources rather than search snippets, and keep stable principles distinct from fast-changing tool instructions.

Research does not authorize autonomous firmware changes. Review it before adopting a curriculum, new mode or claims of effectiveness.

## 5. Receiving-agent checklist

- Inspect current branch, origin and clean/dirty state; fetch and compare without overwriting local changes. Canonical remote is `luminerdy/TrailGuide`, branch `main`.
- Read the contracts above and distinguish their planned features from BRINGUP.md evidence.
- Preserve V1 and the running V2 trial. No reset/upload is necessary to read this handoff; do not interrupt a physical review or agreed soak.
- Do not assume a serial port or private address is still current. When an update is authorized, independently identify the C6 and verify target/status. Keep private values out of tool/chat output.
- Keep `trailguide_v2/secrets.h` and `oled_oracle/secrets.h` ignored and mode 600. Existing local credentials are not part of a fresh clone; obtain them through approved local provisioning, never chat or public commits. Do not regenerate an existing OTA password accidentally.
- Keep runtime team data, private logs, backups and credential-bearing binaries out of Git.
- Use tests before behavioral changes; publish only scoped changes and verify exact remote contents afterward.

## 6. Rebuild the known V2 configuration

Prerequisites: Arduino CLI 1.3.1; Espressif Arduino core 3.3.2; manufacturer-bundled GFX 1.5.9, LVGL 8.4.0 and AXS5106L library. The exact manufacturer archive/checksum, extraction and panel configuration are in V2-BRINGUP.md. A fresh clone does not include the private archive/build outputs or credentials.

From repository root, after those prerequisites are established:

```sh
python3 tools/build_v2_content.py
python3 tools/prepare_v2_panel.py
ROOT="$PWD/build/v2-0.1.8"
SKETCH="$PWD/trailguide_v2"
PANEL="$PWD/build/v2-0.1.0/generated"
LIBS="$PWD/build/v2-c6-bringup/vendor/Arduino/libraries"
FQBN='esp32:esp32:esp32c6:CDCOnBoot=cdc,FlashSize=8M,PartitionScheme=default_8MB,FlashMode=qio,FlashFreq=80'
FLAGS="-I$SKETCH -I$PANEL -DLV_CONF_INCLUDE_SIMPLE"
arduino-cli compile --jobs 2 --fqbn "$FQBN" --libraries "$LIBS" \
  --build-property "compiler.cpp.extra_flags=$FLAGS" \
  --build-property "compiler.c.extra_flags=$FLAGS" \
  --output-dir "$ROOT/firmware" "$SKETCH"
```

The panel helper's generated directory retains the initial trial name; it is board initialization, not a request to restore old firmware. Preserve its manufacturer register sequence despite the Arduino_ST7789 wrapper's name. Supply the same project LVGL configuration to C and C++.

`trailguide_v2/version.h` currently defaults to patch 7. Bump the intended version for a new release; keep screen, serial, status endpoint and updater expectation aligned. Numeric `TRAILGUIDE_PATCH_VERSION` overrides avoid fragile string escaping in build flags.

## 7. Wireless updates and recovery

Only for an authorized update, with the device powered and reachable on the trusted LAN:

```sh
python3 tools/v2_ota_upload.py DEVICE_IP \
  build/v2-0.1.8/firmware/trailguide_v2.ino.bin \
  --expected-version v2-0.1.8
```

This is the known-checkpoint command, not a reason to reflash during handoff. For a new build use its actual path/version. The helper reads the restricted local V2 password file, validates C6/application/slot size, chooses an ephemeral Pi callback port and confirms the running version/continued OTA readiness remotely. Do not send merged images, bootloaders or V1/S3 applications.

Proven checks include wrong-password rejection, authenticated partial-transfer recovery, offline navigation/reconnect and multiple real wireless updates with active-slot/version readback. This does not prove automatic rollback, arbitrary power-loss safety or future stored-data retention.

OTA uses UDP 3232 plus a TCP callback to the host. Fixed callback ports can hit TIME_WAIT; the V2 helper selects a free ephemeral port by default. The read-only `/status` endpoint exposes version/readiness/uptime/slot metadata, not secrets or team data.

This is authenticated trusted-LAN updating, not TLS-encrypted or signed-firmware deployment. Do not expose it to the internet. Never remove power during Updating. USB recovery remains available; original factory full-flash backups were read twice and matched. See V2-BRINGUP.md before restoring anything. Restoring a USB-only application removes OTA until another OTA-enabled image is installed.

## 8. Tests and remaining acceptance

Current v2-0.1.8 evidence: thirty-four Python tests and actual-LVGL 8.4 ASan/UBSan passed; both modes/all eight, coherent face/word movement, exact timing/settled handoff, complete text/scrolling, input-driver hold/release/drag, rollover, OTA, allocation failure and independent deletion/teardown were exercised. The host recorded 422 caller allocations with none remaining and stable heap over 100 cycles; those are not target heap/frame-time measurements. Independent review passed. C6 compilation reported 1,427,064 application bytes / 113,372 globals; unchanged partitions and application image/hash were checked. One authenticated OTA installed v2-0.1.8 in 0x10000, changed from 0x340000; exact running version, OTA readiness and increasing uptime were read back. No C6 USB diagnostic or physical motion acceptance is claimed.

```sh
python3 tools/build_v2_content.py --check
python3 tools/build_prototype.py --check
python3 tools/build_v3_experience.py --check
python3 -m unittest discover -s tests -p '*_test.py' -v
python3 tools/test_v2_ui.py
g++ -std=c++17 -Wall -Wextra -Werror tests/v2_model_test.cpp -o "$TMPDIR/trailguide-v2-model-test"
"$TMPDIR/trailguide-v2-model-test"
g++ -std=c++17 -Wall -Wextra -Werror -I tests/network -I tests/stubs tests/v2_ota_test.cpp -o "$TMPDIR/trailguide-v2-ota-test"
"$TMPDIR/trailguide-v2-ota-test"
```

Run V1 suites from AGENTS.md when relevant; V1 is not a flash target for V2 work. Remaining gates include revised-copy readability, actual touch/drag/edge behavior, exact PCB identification, sustained memory/network soak, restart and independent-power endurance. Future history additionally needs atomic durability, attribution, migration, corruption and update-retention tests with synthetic data.

## 9. Next steps and definition of done

1. Choose one meaningful project and next useful milestone. Use the draft mentor practice and let the learner choose the AI role/help style or a non-AI route.
2. Try that piece, inspect what happened, request voluntary feedback and offer a portable continuation brief. Review evidence where it informs this decision; no hidden inference or AI-use target.
3. Review the V3 study's pacing and feel separately. Before physical implementation, verify the exact board/IMU, handling suppression, rest/cooldown, cancellation, touch/reveal accessibility and actual screen performance. Browser simulation is not a hardware pass.
4. Retain the pending V2 human guide/readability/touch check without resetting/reflashing for documentation. Review existing eight paired cards before expansion.
5. Choose the smallest useful next increment. Integrated AI coaching, automated role-play and situation UI remain separately scoped; production eligibility/pools and durable Team Trail retain existing release gates. Do not enable recording before storage is honest and tested.

Research/publication done means an actual cited artifact exists and is reviewed, not merely a scheduled task. Firmware done means tests, target compilation, authorized transfer and appropriate running-version verification; human learning/readability claims additionally need human evidence. Keep the device stable while research and documentation proceed.
