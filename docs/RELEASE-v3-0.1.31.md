# Current installed source — v3-0.1.31-mode-switch

## Scope

Installed DRAFT trial, not production certification. Classic main cards and Guide have a destination button: **Try** on Ask, **Ask** on Try. It runs the existing opposite-mode action exactly once, drawing independently from that mode's deck and opening its main card. It is not a matching-card face toggle. Another and completed gestures retain that selected mode. Ordinary Guide/Back keeps the current card. Main: Home / Try or Ask / conditional More text. Guide: Back / Try or Ask / conditional More text. No passive Ask label.

The three top targets are 64px wide at x=76/148/220, y=54. Complete-copy scrolling remains available. Coaching stays unchanged, including **Try (optional)** and **Question**; see its [display guide](COACHING-DISPLAY-GUIDE.md), [printable guide](COACHING-DISPLAY-GUIDE.pdf) and [flow](COACHING-FLOW.md). [Classic flow](CLASSIC-FLOW.md) and its [display checklist](CLASSIC-DISPLAY-CHECKLIST.md) describe the independent decks separately.

Only `ui.h` changes among the 17 owned runtime files relative to frozen28. Authored/generated words, 20/18px fonts, 6000ms whole-face reveal, separate 2000ms settled card-only hold, gesture/sensor policy and optional shared RAM-only boot-OFF sound are unchanged. Classic remains the startup default. Touch can finish the same reveal/start its hold, then expose controls; dragging is not tapping.

## Source identity and evidence limits

[`content/v3-current.json`](../content/v3-current.json) pins all 17 owned and all 29 prepared files. The frozen28 [`content/v3-baseline.json`](../content/v3-baseline.json) and tag `v3-0.1.28-controls` remain unchanged; use that tag for the [historical28 recipe](BASELINE-v3-0.1.28.md).

Installed application: **695,776 bytes**, SHA-256:

```text
0f41758346f19d823109f956b01cb6bf576700f23d5888d3ab588fe6598a92d4
```

**REUSED original31 evidence:** fresh 487-command SDK build; closed 147-command native suite with 27 C++ builds, 424 verified HOST C objects, 11 protocol tests, zero skips and 547 frame comparisons. Of those, 359 frames were byte-exact and 188 changed only within the approved top-control region (33 main, 155 Guide); all 547 were exact outside that region. Independent native review: 170 cases / 1911 checks. Complete application readback before boot and strict two-snapshot initialized-UI/advancing-sensor runtime checks passed; execution resumed. These are retained installation checks, not new hardware operations during publication.

Publication freshly checks 86 affected tests with zero failures, errors or skips, four generators and complete source preparation against the installed SDK sketch. A fresh host C++ protocol producer is compiled and executed before the integration-tier decoder checks, so all 11 protocol cases run without skips. This establishes source identity, **not a relocated bit-identical binary rebuild**. The complete historical native runner still depends on private older fixtures; no new portable execution of that full suite is claimed. Binaries, ELF/maps, raw captures, deployment tooling and device identifiers stay private.

Qualitative acceptance of the new flow is limited to one unit and is not exhaustive gesture/readability coverage. **OPEN:** inherited Coaching compass/path automatic-shake gaps and the original turnover-stuck-compass/blank cause. No motion-policy fix, new field movement trace or learning-outcome evidence is claimed.

## Fresh public preparation — no compile or upload

Use the pinned setup described in [V3 bring-up](V3-BRINGUP.md): Arduino CLI 1.3.1, Arduino-ESP32 3.3.2, original-family drivers with the reviewed DMA adapter and bundled LVGL 8.3.10. Obtain the original vendor archive separately at `build/v3-s3-bringup/ESP32-S3-Touch-LCD-1.85-Demo.zip`; it is not redistributed. Required SHA-256:

```text
fa6cd4c47923d559e15571a20ec0d7029febee76e14e6afbd99760f9ad5d6990
```

From a clean current-source checkout with a fresh disposable destination:

```sh
python3 tools/build_v3_content.py --check
python3 tools/build_v3_coaching.py --check
python3 tools/build_compass_coaching.py --check
python3 tools/check_v3_baseline.py --manifest content/v3-current.json
python3 -c 'import sys; from pathlib import Path; sys.path.insert(0, "tools"); from prepare_v3 import prepare; prepare(Path("build/v3-0.1.31/public-local"))'
python3 tools/check_v3_baseline.py --manifest content/v3-current.json --prepared build/v3-0.1.31/public-local/prepared/trailguide_v3
```

The checker defaults to current when present, otherwise frozen baseline; `--manifest` selects explicitly. Its strict flat 17/29-file inventory rejects extra directories, symlinks, nonregular nodes and changed bytes. The preparation API accepts only its canonical31 root or descendants, rejects older release/private-motion roots before archive access and never compiles or uploads. Never reuse an accepted output. Inspect generated `preparation.json` before any separately authorized build: its FQBN retains hardware CDC, 16MB flash, OPI PSRAM, QIO and `app3M_fat9M_16MB`, plus the recorded C/C++ LVGL configuration flags. Preserve the pinned SDK's effective `-mdisable-hardware-atomics` option. Source publication is not deployment or recording permission.
