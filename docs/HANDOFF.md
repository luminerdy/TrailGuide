# TrailGuide — project and coaching handoff

## BLUF

TrailGuide's main effort is **project-led mentoring across varied AI uses**, for individuals and teams, and helping learners continue without depending on the mentor. Start with something meaningful to make, investigate or improve; use the next useful piece to focus learning. Research and building can be learning activities even when the artifact contains no AI. The physical device remains an optional, enjoyable companion and building experiment—not the curriculum or a constraint on all materials.

Draft mentor practice and a learner-owned project/coaching brief connect to **AI Learning Lab's Define → Direct → Check → Adjust** and flexible AI roles. The next primary step is to use/review them on one real project milestone, inspect usefulness and carry context into a next conversation. They are not a validated curriculum or an integrated AI-coaching service.

The separate V3 experience study explores **shake → one random curated idea → gradual text reveal**, with simulated shake, touch fallback, Reveal all and Instant text. Actual V3 motion firmware and human/hardware acceptance remain pending. The last verified V2 checkpoint remains **v2-0.1.5** with authenticated local Wi-Fi updates; this work does not flash, reset or modify V1/V2.

This handoff records a checkpoint, not a guarantee of live hardware availability. Inspect the current Git commit/status for the exact source revision. No firmware changes or canonical card revisions/review promotions are part of this direction/experience increment.

## Read in this order

1. [README — vision versus implementation](../README.md).
2. [Project-led mentoring direction and examples](COACHING-DIRECTION.md), then the [portable learner-owned project/coaching brief](PROJECT-COACH.md).
3. [Canonical product requirements](PRODUCT-SPEC.md), especially P-19 purpose and optional P-20 experience, alongside device P-16/P-18.
4. [Delivery plan](PLAN.md) and [actual verification record](BRINGUP.md).
5. [Two-mode content/navigation contract](FACILITATOR-MODES.md).
6. [V2 trial build/review](V2-TRIAL.md), [V2 Wi-Fi/OTA](V2-WIFI-UPDATES.md) and [original-image recovery](V2-BRINGUP.md).
7. [V3 experience study and hardware boundaries](V3-EXPERIENCE.md), separate from the retained full-flow round preview.
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

### V2 — current development target

ESP32-C6 1.47-inch, nominal 172x320 portrait capacitive touch, pin-header family. C6 silicon revision 0.2 and 8MB flash were probed; exact physical PCB/variant confirmation remains open. Manufacturer-demo operation was human-reported working. esptool package labels differ by version; do not substitute either label for physical identification.

Running checkpoint: v2-0.1.5 DRAFT, installed over Wi-Fi and remotely version/OTA-readiness verified.

Implemented trial:

- Home: Ask the Team and Try a Move, no automatic draw on fresh boot.
- Eight canonical draft pairs, one per theme; fixed review-order Another rather than production shuffled/filterable pools.
- One Guide me entry; Back preserves the card. Complete copy is vertically scrollable.
- Ask: What to listen for → direct cue → You could ask.
- Try: How to run it → What to notice → You could ask → optional If helpful, offer.
- Redundant leading Listen for / Look for / Notice is removed from displayed cue bodies only. Remaining wording, negations/cautions, mixed-case names and acronyms are retained.
- Optional station Wi-Fi, background reconnect, authenticated local OTA, updating input-blocking overlay and a minimal read-only status endpoint. Guidance remains usable offline.

Not implemented: theme picker, production eligibility/pools, linked-face shortcut, team roster, saving/recording, durable Team Trail, facilitator practice mode, live AI, learner monitoring or cloud updating. Reserved partitions or tests must not be mistaken for those features.

### V3 — future round target

Separate Waveshare ESP32-S3-Touch-LCD-1.85, nominal 360x360 round. Its original full-flow circular browser preview is retained for V3/shared design, not C6 hardware emulation. That artifact still has older guidance order and requires separate alignment/review. Do not reuse C6 binaries, partitions or geometry blindly.

The independent `prototype/v3-experience.html` study uses the same eight draft pairs and a simulated shake to explore shuffled ideas, settling and gradual reveal. It is offline, read-only and without sensor APIs, tracking or persistent preferences. The simplified screen and outside-circle review controls do not implement Guide me, themes, team history or the full P-17 flow. See V3-EXPERIENCE.md for build/tests and the actual-board IMU/reveal gates. Preserve touch parity and optional instant text; no gesture or reveal counts as use. Novelty is not proof of usefulness or learning.

### Content

`content/cards.json` is canonical: forty draft cards across eight themes, five per theme. Eight selected cards have paired facilitator guidance; the other thirty-two do not. Ten existing cards also have optional CLI-agent adaptations; they are not extra cards or another theme. Structural tests are not editorial approval.

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
ROOT="$PWD/build/v2-0.1.5"
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

`trailguide_v2/version.h` currently defaults to patch 5. Bump the intended version for a new release; keep screen, serial, status endpoint and updater expectation aligned. Numeric `TRAILGUIDE_PATCH_VERSION` overrides avoid fragile string escaping in build flags.

## 7. Wireless updates and recovery

Only for an authorized update, with the device powered and reachable on the trusted LAN:

```sh
python3 tools/v2_ota_upload.py DEVICE_IP \
  build/v2-0.1.5/firmware/trailguide_v2.ino.bin \
  --expected-version v2-0.1.5
```

This is the known-checkpoint command, not a reason to reflash during handoff. For a new build use its actual path/version. The helper reads the restricted local V2 password file, validates C6/application/slot size, chooses an ephemeral Pi callback port and confirms the running version/continued OTA readiness remotely. Do not send merged images, bootloaders or V1/S3 applications.

Proven checks include wrong-password rejection, authenticated partial-transfer recovery, offline navigation/reconnect and multiple real wireless updates with active-slot/version readback. This does not prove automatic rollback, arbitrary power-loss safety or future stored-data retention.

OTA uses UDP 3232 plus a TCP callback to the host. Fixed callback ports can hit TIME_WAIT; the V2 helper selects a free ephemeral port by default. The read-only `/status` endpoint exposes version/readiness/uptime/slot metadata, not secrets or team data.

This is authenticated trusted-LAN updating, not TLS-encrypted or signed-firmware deployment. Do not expose it to the internet. Never remove power during Updating. USB recovery remains available; original factory full-flash backups were read twice and matched. See V2-BRINGUP.md before restoring anything. Restoring a USB-only application removes OTA until another OTA-enabled image is installed.

## 8. Tests and remaining acceptance

Earlier v2-0.1.5 evidence: nineteen Python tests passed, actual-LVGL host UI checks passed with ASan/UBSan, C6 compilation succeeded, and the running version/OTA readiness were read back after wireless installation. Earlier V1 and V2 network/disabled-config suites passed as recorded in BRINGUP.md. Current project/experience checks are recorded separately there; no new compilation or device-version readback is implied by this increment. Host/browser/synthetic tests are not human acceptance.

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
