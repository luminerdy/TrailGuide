# Frozen working baseline — v3-0.1.28-controls

## Status and acceptance boundary

**Historical frozen working baseline and preserved reference: `v3-0.1.28-controls`.** This source-and-documentation checkpoint is identified by tag `v3-0.1.28-controls`. It promotes the exact installed28 source, not an experimental gesture overlay. It remains a DRAFT offline authored-content trial, not stable-production certification. Verify the tag and its source manifest when checking out or comparing future work.

The installed application is **695,728 bytes**. Complete application readback matched SHA-256:

```text
c25be6578548b6d0f9e57ea74b4c9fb4385dced139d0f83ddf9c8d96f8c4a0ad
```

Exact-version ROM and archived ELF/map binding, initialized Classic UI, advancing valid real IMU/application counters, boot mute and empty/off recorder passed strict runtime checks; execution was resumed. These are retained installation results, not new hardware tests from baseline promotion. Build and source reproducibility are separate from installation/readback and physical acceptance. A locally rebuilt image is not automatically the accepted installed binary or hash.

Qualitative **“Works Great”** feedback accepts the revised timing positively. Mode and trial count were not specified. Do not infer both-mode timing trials, new gesture acceptance, complete readability/touch coverage, sustained reliability or learning outcomes.

## Frozen interaction and content

- Six-second (6000ms) whole-face reveal, then a separate two-second (2000ms) settled card-only hold in Classic and Coaching. The wait starts at reveal settlement, not initial appearance. No additional timed black pause.
- Touch remains sufficient: same-card tap finishes reveal/starts hold; a subsequent settled tap exposes controls. Dragging is not tapping. Guide/Back do not consume another draw.
- Classic boots by default. Coaching-first is product direction, not permission to remove Classic or change startup.
- Plain, unboxed S retains its real touch target and eight-second explicit shake-intent window with Shake ready feedback; it is a fallback, not proof automatic shaking works everywhere.
- ±16g accelerometer configuration/conversion, existing gyro configuration, global Start Fresh, one-draw episode safeguards and touch/scroll suppression are retained. Baseline promotion adds no gesture repair.
- Sound is shared, optional, RAM-only and OFF at boot. No microphone/RX, networking or OTA is added.
- The seven approved wording fields shipped in23 remain in28 with matching authoring/generated inputs. Catalog identities, schemas and draft review states are preserved. See [Delivery B](DELIVERY-B-CONTENT.md); the deferred reference guide and usability proposals are not included.

## Known issues and evidence limits

**OPEN:** inherited27 Coaching-compass shaking failure and intermittent automatic shaking after path selection. S then shake worked in that reported path-selection situation. Classic compass/card and Coaching card shaking also had positive reports; those partial observations do not prove reliability or exactly-once behavior across screens. The original turnover-stuck-compass/blank cause remains open. There is no27/28 field movement trace attributing these failures. The rejected pair-evidence experiment is explicitly excluded.

Old27 observer host passes are not live readiness: two actual starts stopped before Record/Go, one mislabelled an IMU-validity failure as wrong context. Full read/resume subsequently confirmed healthy running state. A future observer must first be rebound and reviewed against exact28 application/ELF/map, followed by separately authorized recording and readiness. No new device operation or recording is authorized by freezing or publishing source.

See [progress and verification](V3-PROGRESS.md) and [bring-up evidence](BRINGUP.md). Older22/27 ledgers are historical, not current source identity or new28 acceptance. Exact recovery27 artifacts remain archived; no automatic rollback is implied.

## Public source and private artifacts

The baseline is the exact installed28 owned runtime source with matching authored JSON, generated consumers, scoped tests and preparation tools. The [source manifest](../content/v3-baseline.json) pins all 17 owned files and all 29 prepared sketch files; the [source checker](../tools/check_v3_baseline.py) rejects differing bytes. It establishes source identity, not an independently certified binary. Source publication excludes binary/ELF/map packages, raw captures/logs, device identifiers, credentials and deployment tooling. Public build instructions are not permission to deploy.

## Pinned setup and off-device preparation

Use Arduino CLI **1.3.1**, Espressif Arduino core **3.3.2**, original-family vendor drivers and bundled **LVGL 8.3.10** with the reviewed display DMA adapter. This tested minimal subset is not manufacturer certification for all peripherals or board revisions. See [original-family setup and archive provenance](V3-BRINGUP.md).

Obtain the original-model `ESP32-S3-Touch-LCD-1.85-Demo.zip` separately; it is not redistributed. The existing public [preparation module](../tools/prepare_v3.py) expects it at repository-relative `build/v3-s3-bringup/ESP32-S3-Touch-LCD-1.85-Demo.zip` and checks SHA-256:

```text
fa6cd4c47923d559e15571a20ec0d7029febee76e14e6afbd99760f9ad5d6990
```

Check out `v3-0.1.28-controls` first (`git switch --detach v3-0.1.28-controls`); do not run this historical recipe against current31 source. Run from that clean checkout, with those dependencies already available. Use a **fresh disposable output**, never an accepted artifact directory. The module defaults to `build/v3-0.1.28` and rejects accepted older release/private-motion roots. Its public Python API also supports an explicit output argument; there are no command-line output/ZIP flags. Check generated consumers and owned source first, then prepare and check the complete sketch:

```sh
python3 tools/build_v3_content.py --check
python3 tools/build_v3_coaching.py --check
python3 tools/build_compass_coaching.py --check
python3 tools/check_v3_baseline.py
python3 -c 'import sys; from pathlib import Path; sys.path.insert(0, "tools"); from prepare_v3 import prepare; prepare(Path("build/v3-0.1.28/baseline-local"))'
python3 tools/check_v3_baseline.py --prepared build/v3-0.1.28/baseline-local/prepared/trailguide_v3
```

Preparation validates the pinned ZIP and emits owned sources, derived drivers/LVGL and `build/v3-0.1.28/baseline-local/preparation.json`; it neither compiles nor uploads. Inspect the emitted `fqbn`, `own_files`, archive pin and `compile_command` before separately authorizing a local build. The explicit generic S3 recipe retains 16MB flash, OPI PSRAM, QIO, hardware CDC and `app3M_fat9M_16MB`; do not substitute a named-board preset or another target's partitions. Preserve the pinned SDK's effective `-mdisable-hardware-atomics` option. The manifest's `compile_command` is the public tool's generated portable argument list, not a private deployment helper. Do not reuse accepted outputs or assume bit-identical binaries across environments.

The source-baseline check freshly exercised public preparation and matched all 29 prepared source hashes to the installed build. Generator, preparation-safety, affected host and fresh producer/protocol checks were also executed. The closed 147-command native suite and 487-command SDK build are retained, source-bound installation evidence, **not newly rerun builds**. The full historical native/isolation runners still require local older baseline/frame inputs; portable fresh execution of that complete historical suite is not established. No hardware operation was performed for baseline publication.

## Freeze rule

Preserve this accepted source, installed/recovery artifacts and their evidence without overwrite. Future changes use a new version and separate branch/output; never reuse this tag or relabel experimental overlays as accepted28. Source edits, host passes or a baseline tag do not automatically approve recording, enumeration, serial opening, reset, flash or other device operations. Keep the working device untouched until separately authorized.
