# TrailGuide — project and coaching handoff

## BLUF

TrailGuide is becoming an offline pocket companion that helps facilitators invite useful thinking, listen and offer a small next move during AI workshops. The endorsed direction is to support **both the team's learning and the facilitator's development**, with appropriate—not maximal—trust in AI.

A read-only portrait V2 trial is running as **v2-0.1.5**, with authenticated local Wi-Fi updates verified. The next work is evidence synthesis, facilitator practice material and human review of the existing eight draft pairs—not another hardware port, automatic learner assessment or an unreviewed expansion of menus.

This handoff records a checkpoint, not a guarantee of live hardware availability. Implementation baseline: `fae2af0`. Use the current Git commit for subsequent documentation changes. No firmware changes are part of this handoff.

## Read in this order

1. [README — vision versus implementation](../README.md).
2. [Coaching direction and examples](COACHING-DIRECTION.md).
3. [Canonical product requirements](PRODUCT-SPEC.md), especially P-16, P-18 and planned P-19.
4. [Delivery plan](PLAN.md) and [actual verification record](BRINGUP.md).
5. [Two-mode content/navigation contract](FACILITATOR-MODES.md).
6. [V2 trial build/review](V2-TRIAL.md), [V2 Wi-Fi/OTA](V2-WIFI-UPDATES.md) and [original-image recovery](V2-BRINGUP.md).
7. Repository `AGENTS.md` before taking development actions.

The older [Pi handoff](PI-HANDOFF.md) is V1 host-setup guidance, not the current project status.

## 1. Direction to preserve

**Notice → Invite → Listen → Offer a small next step → Reflect** is a proposed coaching scaffold to investigate, not a proven TrailGuide learning protocol.

- Make room for uncertainty without putting someone on the spot.
- Ask about what a learner tried and noticed before assuming a personal problem.
- Offer a hint, example, thinking time, a small test or a different participation format; passing and declining AI use remain legitimate choices.
- Evaluate a particular AI contribution in its task/risk context. Do not label people as over-trusters, skeptics or weak learners.
- Support the facilitator before, during and after a session. Begin practice outside the live device rather than immediately adding another home mode.
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

Separate Waveshare ESP32-S3-Touch-LCD-1.85, nominal 360x360 round. Its circular browser preview is retained for V3/shared design, not C6 hardware emulation. The round artifact still has older guidance order and requires separate alignment/review. Do not reuse C6 binaries, partitions or geometry blindly.

### Content

`content/cards.json` is canonical: forty draft cards across eight themes, five per theme. Eight selected cards have paired facilitator guidance; the other thirty-two do not. Ten existing cards also have optional CLI-agent adaptations; they are not extra cards or another theme. Structural tests are not editorial approval.

## 3. Guide-writing agreement

Keep the main question/action distinct from the help. A heading already supplies its instruction: under What to listen for, begin with “Alternatives that change the approach…” rather than “Listen for alternatives…”. Follow-ups are optional suggestions, not an interrogation or an automatically chosen branch.

The current formatter changes presentation, not canonical editorial text, IDs, revisions or review states. If authoring changes meaning/canonical copy, increment only affected revisions, preserve stable IDs and synchronize the catalog/tests. Never silently mark drafts reviewed.

Keep the live tool glanceable on 172x320. Preserve one help entry, readable copy, complete overflow access and touch safeguards. Do not solve wording problems with extra menus or claim browser/host rendering proves physical readability.

## 4. Research and training work to pick up

At this handoff, separate private research runs and a morning synthesis were scheduled. Their completion is not assumed. Check actual output/scheduler status before citing conclusions; the current request and research workspace are outside the public repository.

Research questions:

- Which coaching/learning principles support invitation, listening, feedback, useful effort and voluntary participation?
- How should a facilitator support both overreliance and reasonable skepticism without pushing AI adoption?
- What distinguishes better AI-assisted task performance from durable understanding and unaided transfer?
- Which findings come from trials, self-report studies, expert consensus or untested proposals, and where do they fail to transfer to adult workshops?
- What access needs, participation alternatives and power dynamics should shape the pilot?

Required outputs: cited evidence brief, short synthetic practice exercises, a minimal proposed product/content change and an honest human-pilot plan. Register/read original sources; do not convert search snippets into evidence. Keep stable coaching principles distinct from fast-changing tool-specific material.

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

Known v2-0.1.5 evidence: nineteen Python tests passed, actual-LVGL host UI checks passed with ASan/UBSan, C6 compilation succeeded, and the running version/OTA readiness were read back after wireless installation. Earlier V1 and V2 network/disabled-config host suites passed as recorded in BRINGUP.md. Host/synthetic tests are not human acceptance.

```sh
python3 tools/build_v2_content.py --check
python3 tools/build_prototype.py --check
python3 -m unittest discover -s tests -p '*_test.py' -v
python3 tools/test_v2_ui.py
g++ -std=c++17 -Wall -Wextra -Werror tests/v2_model_test.cpp -o "$TMPDIR/trailguide-v2-model-test"
"$TMPDIR/trailguide-v2-model-test"
g++ -std=c++17 -Wall -Wextra -Werror -I tests/network -I tests/stubs tests/v2_ota_test.cpp -o "$TMPDIR/trailguide-v2-ota-test"
"$TMPDIR/trailguide-v2-ota-test"
```

Run V1 suites from AGENTS.md when relevant; V1 is not a flash target for V2 work. Remaining gates include revised-copy readability, actual touch/drag/edge behavior, exact PCB identification, sustained memory/network soak, restart and independent-power endurance. Future history additionally needs atomic durability, attribution, migration, corruption and update-retention tests with synthetic data.

## 9. Next steps and definition of done

1. Complete/review the coaching evidence brief, with limitations and contradictory results visible.
2. Draft a small before/during/after facilitator-practice companion and review the existing eight pairs with humans.
3. Pilot with consent, access alternatives and passing; assess usefulness through agreed observations and voluntary feedback, not hidden inference or AI-use counts.
4. Choose the smallest next content/UI improvement. Keep new practice modes, role-play automation and situation navigation as proposals until separately scoped/tested.
5. Continue production eligibility/pools and durable Team Trail work under the existing release gates; do not enable recording before storage is honest and tested.

Research/publication done means an actual cited artifact exists and is reviewed, not merely a scheduled task. Firmware done means tests, target compilation, authorized transfer and appropriate running-version verification; human learning/readability claims additionally need human evidence. Keep the device stable while research and documentation proceed.
