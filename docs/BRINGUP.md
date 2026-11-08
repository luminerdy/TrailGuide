# Development and verification record

Technical evidence only. Keep this document reusable: no personal identifiers, machine-specific home paths, chat narratives, calendar dates or references to a particular workshop. Frontier Ops remains a reusable theme. Versions and test durations describe technical behavior, not event scheduling.

## V3 accelerometer range correction — v3-0.1.27-accel16

- Current scope and remaining acceptance: [V3 progress ledger](V3-PROGRESS.md). The retained26 ±4g trace has 4,544 rows / 181,760 bytes / validated full CRC, 352 clipped-invalid rows, zero recorded gesture triggers and no new read-error delta. Exact-input replay and independent causal-limits review confirm saturation cancellation for the last known armed attempt, not every physical failure.
- Scope: ±4g → ±16g accelerometer selection, conversion and range-dependent validity bounds; three source headers only apart from build identity. Gyro configuration, gesture timing/threshold policy, UI and unrelated drivers remain unchanged. Older saturated raw data is not reinterpreted as16g evidence.
- Retained software evidence: independent source review; actual candidate147 native commands, 27 fresh C++ commands, 424 verified HOST C objects, 547 exact frame comparisons, 11 protocol cases / zero skips; clean487-command S3 build and independently verified closed application/archived ELF/map/source package. The task-local26 native baseline was reused explicitly. No new isolated128 run is claimed.
- Installer evidence: 24 parent-run guarded offline tests and independent exact-helper review PASS. Actual current26 frozen headers matched the complete retained export before serial/reset, so its validated bytes were reused. Only the695,712-byte application at `0x10000` was written; complete matching readback preceded the native USB boot reset. Compatible bootloader/partitions were not rewritten. Previous26 evidence and recovery artifacts remain retained; no automatic rollback.
- Strict live27 two-snapshot ROM/UI/advancing valid sensor and application checks passed, including plausible-g vectors, Classic boot, sound OFF and empty/off recorder defaults. Execution resumed. Running verifies the checked initialization path, not a separate direct register probe or a physical shake result.
- Human feedback: shake works from Classic compass and Classic card / New; Coaching card shaking can work, but automatic shaking sometimes fails after path selection and S then shake works. Coaching compass shaking does not work. Counts, exactly-once behavior, flip and ordinary-handling negatives remain unestablished. No27 movement recording or further device operation is part of this documentation checkpoint.
- Next: off-device context/eligibility/acquisition/dispatch comparisons and a focused reproducible test, then a scoped proposal before further implementation or installation. Preserve the current device while physical testing is paused.

## V3 Coaching-context follow-up — unchanged installed27 source

- Exact27 build-bound source, actual LVGL pointer input and UI/model dispatch, ASan/UBSan: focused27-scenario packet. Across all six real pointer-selected paths, immediate automatic bursts without renewed acquisition produce zero events; identical bursts after quiet acquisition or a released S tap produce one event and one model draw in each tested route. Classic/Coaching compass/card acquisition-positive controls pass.
- Parent independently verified retained input/file hashes and reran source/fixture/binary-bound mechanism/control and contract loops: mechanism exit0; intentional unchanged-baseline contract RED exit1. Pointer/contact-scroll, fault/stale/gap, recovery and continuous-episode checks have bounded coverage; independent inertia/rebound and broader rearm remain separate.
- This establishes a deterministic requirement counterexample, not the cause of the unrecorded human27 incident or physical exactly-once acceptance. No new firmware, full147/SDK rerun, hardware contact or recording occurred. Original native link attempts exceeded their16 MiB peak envelope; those failures remain recorded. Final replay and independent parent reruns stayed within the replay envelope, not a retroactive pass for linking.
- Findings, causal limits and the unimplemented scoped proposal: [V3-GESTURE-FOLLOWUP.md](V3-GESTURE-FOLLOWUP.md).

### Bounded acquisition/rearm and independent inertia extension

- Fresh sanitized C++ with424 reverified HOST C objects and exact27 build-bound source: five contexts demonstrate ten synthetic initial-acquisition/post-cooldown-rearm gaps under ordinary handling; quiet controls recover, and continuous strong motion remains at one gesture event. This is not a physical trial or attributed incident replay.
- Independent LVGL scroll ownership plus changing post-release position confirms short inertia in Classic and Coaching. No draw occurs during the observed interval, but UI motion eligibility is prematurely true. Track this separate safety gap without claiming it explains missed gestures or establishes longer-inertia safety.
- Eighteen axis/sign/cadence characterization rows use equal sample counts but unequal durations; short-window rejections are not evidence of a cadence defect. Parent verified the closed inputs/earlier packet and re-executed mechanism0 and intentional contract RED1.
- Revised fresh-build32 MiB peak gate passed; workspace footprint and child RAM high-water were tracked separately. Original16 MiB failures remain failures. No firmware/policy/device change, recording or new full-suite/SDK run. The proposal in [V3-GESTURE-FOLLOWUP.md](V3-GESTURE-FOLLOWUP.md) remains unimplemented and requires approval.

### Approved implementation — focused merged PASS, wider acceptance BLOCKED

- Three-header private evaluation overlay: automatic acquisition/episode rearm plus existing Classic/Coaching eligibility predicates. Other26 build-bound source files remain byte-identical. Installed27 remains unchanged.
- The parent freshly compiled the same independently frozen actual-LVGL fixture against the merged overlay: compiler0, mechanism0, contract0; all100 controls and12 mandatory targets PASS. Targets explicitly include Coaching compass entry/return without S or quiet setup, all six paths, smooth motion and ordinary-handling rearm. This is not physical acceptance.
- Independent review separates23 cold40ms triangle reference-admission misses from3 square proof/credit timing misses. A seed-only change cannot close both; wider behavior remains blocked before full native/SDK release gates.
- Held constant-offset and later-full-burst cancellation/expiry tests were classified explicitly, not silently rewritten: eight seconds without oscillation is not uninterrupted shaking, and nine fresh probes show later automatic events equal a never-manually-armed detector with manual intent canceled. Original REDs remain retained. Sensor-producer/health/actual-UI composition remains a separate pending check.

### Producer-health closure and remaining design decision

- Same merged candidate, fresh C++ against real Imu with a fake bus, exact source-extracted sketch health expression and actual Ui/CoachUi:35 cases /518 assertions,0 failures. Parent reverified819 old/source inputs and repeated the sealed native executable. BASE repeats the known policy RED; its246 dependent assertion failures are not246 independent bugs. Original old-range/cancellation/expiry fixtures remain unchanged.
- Includes current16g/gyro safety edges, real status/raw faults, retained/stale timestamps, NotReady, contact/root deletion, exact8000ms expiry, no short-burst credit aggregation, fresh automatic event/model equivalence and continuous/modulated latch checks. Actual int16 producer cannot generate nonfinite values; no nonfinite injection claim or hardware result.
- Design probe supports reference geometry for the23 exact unequal-extrema triangle patterns, not integrated or noisy-motion acceptance. Three square cases expose a raw-proof recurrence conflict and slow-sample credit timing. Conditional adjacent-pair-owned automatic authorization would change evidence semantics and needs approval before the final refinement. No source change, full SDK gate, device operation or new recording follows from this result.

### Automatic pair-evidence experiment — STOP / rejected

- Separately approved automatic-only experiment, Shake-header-only overlay: exact26 old cold misses fixed; broader cold1,440-row matrix regresses to147 previously working failures. Weak-modulation576 matrix has36 failures, including initial-burst misses; retained healthy-NotReady positive also fails. Quiet matrix remains identical, including520 unsupported old positives. No universal latency or physical-success claim.
- Parent ran the independently frozen oracle with fresh sanitizer-enabled C++ against the exact experiment header: primary2,990 checks /148 failures, supplemental42 /0 failures. Header/source manifests and preexisting inputs verified; no oracle edit. Native state204 bytes passes the new256-byte engineering bound but fails the old128 assertion, retained explicitly.
- Experiment rejected; no merge, further production tuning, full native/SDK build, UI/producer recomposition or hardware operation. Earlier focused native/producer passes remain bound to the earlier overlay. Installed27 and unrelated editorial work remain unchanged. Future field capture requires its own prepared observer and permission.

## Historical manual-S moving-start repair — v3-0.1.26-manual-s

- Historical26 implementation and installation evidence follows. Current27 status is in the [V3 progress ledger](V3-PROGRESS.md). Versions25 and26 both failed physical shake testing; those failures are not overwritten by host passes. The subsequent26 trace and range correction are described above.
- Manual acquisition only: moving-start/smooth-phase correction with the eight-second explicit intent preserved. Automatic recognition, UI appearance, drivers, thresholds and duration-credit policy are unchanged. The original human incident was not causally reproduced.
- Retained evidence: 974 strict host checks plus 20 independent boundary checks; separate square/sine matrices of 18,720 scenarios each; focused actual-LVGL 54 cases / 639 checks; full 147 native commands with 27 fresh C++ commands, 424 verified host C objects, 547 frame comparisons and 11 protocol tests / zero skips. No new isolated 128-test claim.
- Clean S3 build: 487 commands and independent package review PASS. Installer: 22 guarded offline tests and focused independent review PASS after a hexadecimal-output retry compatibility correction; strict runtime requirements retained.
- Actual 695,712-byte application-only write at `0x10000`, full matching readback BEFORE native USB boot reset, then exact version26 ROM and strict two-snapshot initialized UI / advancing valid sensor and application checks PASS. Classic boot, sound OFF and empty recorder defaults verified; execution resumed. Previous25 evidence retained, no new recording or automatic rollback. This documentation update does not contact hardware or rerun those firmware checks.

Earlier entries below retain their original version-specific scope, not current27 acceptance.

## V3 qualified-turnover blanking — v3-0.1.11

- Presentation-only existing Inverted getter and root/child visibility masking: full black through return qualification, original one draw/6000ms reveal/5000ms hold, cancellation restoration and real-contact swallowing. No new overlay/allocation/timer, sensor, recognition, model/RNG, card, renderer or recorder change. Mask bound 32/normal≤6 children; unsupported foreign mutation limitations documented.
- Actual ASan/UBSan/native pixel/input/heap/lifecycle checks, full native/content/SDK/differential and 53 isolated Python tests pass; independent 14-file review PASS. All five prior installed outputs unchanged. 13 native frames show exact black stages and byte-identical cancellation restore.
- Actual build 646,191 program/128,192 globals; app 646,336 bytes. App-only write/explicit verify and source/original/same-board/security/image/component gates pass. Exact 1.11 ROM/UI/Home/clear blackout and configured real IMU, live counters/direct sensor memory checks pass. Legacy diagnostic streaming probe yielded no usable rows and was not accepted as successful evidence.
- Human confirmed blank → card animation → other buttons. 4940 natural poll rows/197600 bytes/112.156seconds/fullCRC retain exactly one trigger, zero clipping and no read errors. Debugger only after freeze; final Card/blackout-off/reveal-off/focus-off verified. No reset disturbed the accepted card. This is one observed visual sequence, not a repeated-reliability or physical cancellation test.

## V3 gyro-range correction and natural turnover success — v3-0.1.10

- One checked gyro-range selection derives CTRL3 0x77, +/-2048 dps, 16 LSB/dps and 2044-dps safety in both diagnostic/consume paths. Primary A/C settings agree; suffix remains unidentified. Acceleration 4g/ODR 7/poll 20ms/rest 12dps/dwells/cooldown/UI/gates/cards/renderer/recorder remain unchanged. No shake/turn combination added.
- Actual RED/GREEN conversions, synthetic 600-dps complete cycle and UI integration, retained all-axis/nonfinite/raw-rail/context/timing negatives; 12000 equal-raw narrowly whitelisted plus 12000 equal-physical differential steps pass. Full native LVGL/content/SDK and 53 isolated Python checks and independent 18-file release review PASS. All installed 1.6/1.7/1.8/1.9 files preserved.
- Actual build 645,639 program/128,184 globals; app 645,792 bytes. Target/originals/source/image/compatible components/security and fresh original-board checks pass; app-only write and explicit verify pass. Exact 1.10 ROM version/UI/Home and actual IMU configuration/readback verified, with live counters and plausible stationary physical units.
- One natural full turnover/back produced a visible card, human-confirmed; 3610 real records/144400 bytes/83.068seconds/fullCRC retain exactly one trigger, Turning/Inverted progression, zero clipping and no read errors. Explicit firmware/range provenance retained; no synthetic input/debugger during movement. Frozen-RAM JTAG export occurred afterward; no cleanup reset disturbed the working card. This is one observed success, not sustained reliability or unrelated visual/power/coaching acceptance.

## V3 longer bounded recorder and measured motion failure — v3-0.1.9

- Diagnostic-only 120s/8192x40-byte PSRAM recorder, strict-session idempotent FREEZE/manual reason 3, legacy/new client compatibility and strict unsigned metadata validation. IMU/gesture/gates/fonts/cards/renderer unchanged; UI version only. All 53 isolated Python, native LVGL/content, SDK and differential checks pass; independent review/unblock PASS.
- Actual S3 compile 645,639 program/128,184 globals; app 645,792 bytes. Original-board/security/16MB/originals/compatible image/partition/source gates, app-only write and explicit verification pass. Exact 1.9 ROM/UI/Home/healthy IMU and moving compass/sample counters pass.
- Synthetic target return with 327,680-byte recorder plus 143,360-byte raster succeeds; normal reveal/hold/control restore, manual reason 3 and full 3725-row/149000-byte CRC export with one synthetic trigger. Long serial export errors were not accepted; exact-ELF read-only JTAG retrieved frozen RAM. Evidence exported before deliberate cleanup reset. Not human gesture or physical pixel acceptance.
- Natural turnover capture 3101 rows/124040 bytes/70.862seconds/fullCRC, manual freeze ACK round trip 70ms. Sensor gravity reverses, gyro-X hits raw rails 8 times; invalid-health cancellation sends the cycle to Recover on both legs, no trigger. No new I2C errors or touch gate explains the cancellations. Physical flip remains failed; wider gyro range is a subsequent testable correction, not yet installed at this checkpoint. Raw traces remain private.

## V3 approved twelve-card integration — v3-0.1.8

- Exact twelve endorsed openings/cues/follow-ups/help integrated into existing canonical IDs;12 revisions advance once, 28 other records unchanged and all scopes draft. Two documented 003/014 activity clarifications preserve genuine AI-assisted evidence checking and a complete human-only option. V3 bags derive count 12; V2/browser source pools remain 8; no V2/V1 hardware update.
- Four generator freshness checks, 45 isolated Python checks, actual LVGL 48 contexts/179 complete pages, six-second reveal/five-second hold, expanded fair bags, legacy RNG, SDK positive budget and recorder/differential regressions pass. Independent content and merged reviews PASS. Prior 1.6/1.7 installed artifacts preserved.
- Actual build 645,507 program / 128,184 globals; app 645,648 bytes. Source/current-prepared headers, image/slot/hash/components/originals and fresh same-board identity validated; app-only write/hash and exact startup/UI/buses/real IMU readback passed.
- On actual target, explicitly synthetic detector-state injection completed return and selected tg-004; live ROM opening exactly matches the endorsed question. Real render/hold/restoration state completed; full 1,904-row/76,160-byte CRC f09b5d32 recorder export contains exactly one synthetic trigger, no read errors/clips. No human flip or physical pixel acceptance inferred. Evidence exported, then intentional reset returned Home/index-1/compass; all monitors closed. Private proof: `build/v3-0.1.8/`.

## V3 bounded poll recorder — v3-0.1.7

- Diagnostic-only PSRAM capture, default OFF, fixed 2304×40 bytes and 45-second bound; no card/model/threshold/reveal changes. Single RX parser, finite positive USB budget and bounded CRC/session/token dump retain evidence without a live reader. Raw invalid/clipped values are diagnostic only, never valid published samples.
- Actual LVGL ASan/UBSan, SDK budget regression, 12,000 differential driver/detector steps and 42 isolated Python checks pass. Numeric-parser review blocker was reproduced then fixed with strict decimal checked uint32 parsing; targeted independent review PASS. All 2697 installed v1.6 files/inventory preserved.
- Actual build 640,967 program/128,176 globals; app 641,120 bytes. Fresh target/backup/image/hash/source/compatible-component gates and app-only write/hash verified; exact startup/UI/actual IMU verified.
- Fresh RECORD session acknowledged nonzero polls; reader closed 46seconds, then actual fetch/verify recovered 1,969 records / 78,760 bytes with full CRC32 d799012b and no reset. Every unattended baseline row valid/eligible/Ready, zero read errors/clips/resets, poll gaps 20–36ms. No scripted human motion: physical flip remains unresolved. Recorder frozen and reader closed.
- Private proof: `build/v3-0.1.7/`; [V3-TRIAL.md](V3-TRIAL.md) contains current protocol/build boundaries. Approved 12-card overlay is not yet installed.

## V3 opposed-reference recovery correction — v3-0.1.6

- Coordinated v1.5 trace: 315 fresh valid/resting eligible rows stuck in Recover, gravity alignment about -0.989 to retained origin, no trigger. Recovery now permits a fresh stable opposed reference after existing 1200ms; same-origin 600ms remains. Reacquisition itself never draws; active phases/thresholds and all UI/content/renderer remain unchanged.
- Old-code synthetic/private 315-row replay RED; corrected actual UI/Gesture/LVGL replay GREEN and subsequent synthetic whole flips exactly one draw. Existing false-positive/busy/nonqueue checks retained. Real LVGL ASan/UBSan, pinned SDK USB-budget regression and 38 isolated Python tests passed; independent review PASS. All 5,516 installed v1.5 files/inventory preserved.
- S3 build 638,671 program/127,640 globals; app 638,816 bytes. Actual chip/hash/slot/source/originals/same-board identity and compatible components validated. App-only0x10000 write/hash verified; startup/UI/bus/real IMU and first healthy heartbeat read back. Large ordinary heartbeat fragments were not treated as complete records.
- Reader closed for ten seconds: direct debug snapshots compass ticks 400→463, UI/sample timestamp 64623→74668ms, motion healthy/Ready and finite TX 1ms. Debugger briefly halted/read/resumed without reset/flash/memory-write commands. Monitors closed. Physical corrected full cycle and card-only timing remain pending. See [V3-TRIAL.md](V3-TRIAL.md); private evidence `build/v3-0.1.6/`.
- Historical v1.4 diagnostic freeze was independently proved by actual GDB loopTask inside HWCDC::write after timeout 0 underflow. Finite positive setup/activation budget fixed this in v1.5 and is retained. This is separate from gesture recovery and not evidence of physical gesture success.

## V3 plain compass and settled card-only hold — v3-0.1.3

- The prior v3-0.1.2 turn-over/back interaction was reported working on the physical device. Its detector/model/IMU/reveal/content remain unchanged. New Home has only a full-face rose centered (180,180), with tap-to-Options and safe-circle hit-testing; no instructions/buttons or idle draw.
- Card chrome hides during the accepted six-second reveal, then for five seconds after settlement. Tap can finish the same reveal/start hold, then expose controls; drag does not. New complete flips remain eligible during the settled hold. Separate deadline ownership prevents stale unhide after new draw/navigation/deletion. Null-timer and real LVGL exhaustion paths leave controls accessible.
- Actual LVGL 8.3.10 ASan/UBSan passed; caller allocations 163, outstanding 0, peak 14,760/49,152. Twenty synthetic flips across mode bags/boundaries yielded different adjacent IDs and exact RNG/model progression. All 38 isolated Python tests passed. Independent scoped review found no blockers. All 5,491 installed baseline files/inventory stayed byte-identical. Host checks are not device timing acceptance.
- Actual S3 build 634,659 program bytes / 126,792 globals; exported app 634,800 bytes. Image/source hashes, slot/target, unchanged bootloader/partitions, original backups and fresh same-board S3/16MB validated. Only app0 at 0x10000 written; data hash verified.
- Exact v3-0.1.3/UI ready/expander-touch ACKs/real IMU identity/configuration/fresh samples read back. Six continuing heartbeats had no sampling errors; Home LVGL free 43,560 and PSRAM free 8,382,632 bytes stayed stable. No synthetic device input or V1/V2 access. Monitor closed. Plain-compass/new card-only timing physical review pending; bubbles and twelve-card draft not included.
- Current private package/logs: `build/v3-0.1.3/`. See [V3-TRIAL.md](V3-TRIAL.md). Source publication remains pending GitHub authentication.

## V3 full-screen compass and turn-over/back — v3-0.1.2

- Full-screen decorative rose with indefinite independent-RNG idle animation, bounded needle dirty regions at 160ms, tap-to-Ask once on release and compact Options for Ask/Try/Instant/Back. No automatic draw or card-RNG use. Accepted answer renderer/model/IMU/canonical content remain unchanged; proposed 12-card copy and bubble preview are not installed.
- Replaced physically unsuccessful shake with stable gravity-reference → held opposition → held return. Synthetic axes/sign/cadence/gap/cancellation/cooldown tests pass; actual turn-over reliability remains unaccepted. Normal faster-loop duplicate samples initially prevented arming: actual integrated RED reproduced it, then fresh-frame dispatch plus immediate context cancellation passed GREEN.
- Independent review blocked a compass rectangle tap outside the radius 168 safe circle. Real input at (180,8) reproduced RED; advanced hit-testing in screen coordinates passed GREEN and targeted re-review. Original background contact suppression and center release-only draw remain intact.
- Actual LVGL 8.3.10 ASan/UBSan, model/gesture/IMU and 38 isolated Python tests passed. Host caller allocations 135, outstanding 0, LVGL peak 14,752/49,152. A sandbox initially lacked original comparison drivers; copying the pinned fixtures resolved that environment error. No accepted fallback was prepared in the original checkout.
- Fixed S3 build 634,199 program bytes / 126,776 globals; exported app 634,352 bytes. Actual S3 header/checksum/hash/slot, current prepared sources, original backup hashes, fresh same-board S3/16MB and byte-identical bootloader/partitions passed validation. Only app0 at 0x10000 was written and data-hash verified.
- Readback: exact v3-0.1.2, UI ready, expander/touch ACKs, physical IMU 0x6b/WHO 0x05/revision 0x7c/configuration/fresh samples. Six continuing heartbeats had no sampling errors; LVGL free 42,808 and PSRAM free 8,382,632 stayed constant. No synthetic hardware input, V1/V2 access, raw-motion handler, telemetry/history, network or optional peripherals added. Monitor closed; physical compass/flip check pending.
- Discontinued shake-diagnostic source fragments were archived privately and are not part of the prepared or installed image. Current private evidence: `build/v3-0.1.2/`; see [V3-TRIAL.md](V3-TRIAL.md).

## V3 playful compass intro — v3-0.1.1

- Home now shows a gently wandering decorative idea compass; no measured north/heading claim or new sensor. Idle ticks use separate pseudo-random state and never draw cards or consume card RNG. Intro shake defaults to Ask through the existing single-draw path. Touch Ask/Try/Instant remain available; Instant text bypasses card reveal, not compass motion.
- Separate compass timer is canceled on navigation, independent compass deletion, root teardown and owner destruction; card-reveal busy state is independent. Existing six-second answer-face renderer, model, gesture thresholds, canonical content and all V1/V2/diagnostic sources are unchanged. No coaching-review recommendations were implemented.
- Actual LVGL 8.3.10 ASan/UBSan verified changing compass pixels, stable idle/model/RNG, synthetic intro shake, contact suppression, touch/Instant, safe-circle controls and teardown/restart. Thirty-eight Python tests passed; host caller allocations 134, outstanding 0, LVGL peak 14,792/49,152 bytes. Independent read-only scoped review found no blockers. Host simulation is not physical performance or shake acceptance.
- Actual S3 compile: 632,967 program bytes, 126,736 globals; exported app 633,120 bytes. S3 chip 9/checksum/hash/slot, original backups and byte-identical bootloader/partitions passed validation. Fresh same-board identity preceded app-only write at 0x10000; data hash verified. Accepted v3-0.1.0 inventory 3,005 files remains byte-identical; an early host preparation re-created identical files and may have changed timestamps, not bytes.
- Exact v3-0.1.1 startup/UI ready, expander/touch ACKs and real IMU identity/configuration/fresh samples were read back. Six continuing heartbeats had no sampling errors; Home LVGL free 41,372 bytes and PSRAM free 8,382,632 bytes stayed constant. No synthetic device taps or shakes were sent. Physical compass/intro-shake behavior remains unaccepted; the prior answer animation was positively observed and its implementation is unchanged.
- Current private package/logs: `build/v3-0.1.1/`. See [V3-TRIAL.md](V3-TRIAL.md). No V1/V2 hardware accessed.

## V3 first read-only card/shake trial — v3-0.1.0

- Added separate owned `trailguide_v3/` and preparation/host tooling, preserving all V1/V2/diagnostic sources and recovery images. Home has no boot draw; canonical eight-pair Ask/Try bags shuffle independently without cycle-boundary repetition. Guide/Back, More text/swipe and memory-only Instant text preserve complete DRAFT copy. No recording/history, accounts, networking/OTA, audio/microphone, RTC or SD is initialized.
- Native six-second wholeface/words use one bounded 143,360-byte PSRAM-only source/output allocation outside 48KiB LVGL heap. Safe instant fallback is tested but is not working motion. Gesture uses fresh 20ms samples with experimental multi-peak/rest/cooldown thresholds; Home/Guide/reveal/pointer/inertia drop gestures without queuing. No physical shake acceptance is inferred.
- Actual LVGL 8.3.10 ASan/UBSan, model/shuffle, synthetic gesture and fake-bus suites passed; 37 Python regressions passed. Host recorded 133 caller allocations with none outstanding and 14,752-byte LVGL peak. Review reproduced a pre-init TLSF crash and incomplete root/inertia suppression; guarded heartbeat and independent pointer/scroll gates passed actual RED/GREEN regressions and targeted independent re-review.
- S3 compile reported 631,943 program bytes / 126,712 globals; exported app 632,096 bytes. Actual chip 9/checksum/hash/slot and unchanged bootloader/partitions were validated. After fresh same-target S3/16MB/port/backup checks, only app0 at 0x10000 was written and hash-verified. No C6 access occurred for this upload.
- New version v3-0.1.0, continuing UI-ready heartbeat, expander/touch ACKs, actual sensor 0x6b/WHO 0x05/revision 0x7c/configuration and fresh sample-valid state were read back. Five Home heartbeats had zero sampling errors, LVGL free 41,788 bytes and PSRAM free 8,382,632 bytes. This is idle/available-memory evidence, not target raster allocation, frame rate or physical pixel/input/shake acceptance. No synthetic device taps or shakes were injected.
- New UI/readability/overflow/controls, actual reveal allocation/smoothness, deliberate shake versus ordinary handling, full-screen mapping, PSRAM health and sustained operation remain separate. Exact payload/logs are private under `build/v3-0.1.0/`; [V3-TRIAL.md](V3-TRIAL.md) is the operating/build contract.

## Earlier V3 optional IMU diagnostic installation — v3-bringup-0.1.1

- Preserved the physically checked button/counter/fonts and added bounded optional IMU diagnostics plus a circle-safe status line. No audio, microphone, SD, RTC, Wi-Fi, storage, recording or shake drawing is initialized.
- Original-family configuration/range behavior and locally pinned supplementary identity/status meanings were checked. WHO_AM_I register 0x00 is separate from revision 0x01; unknown/missing devices are not configured. Reads/configuration are bounded and read back; short/stale/invalid frames suppress numeric samples. Native axes are uncalibrated, not mapped to the screen; no uncertain software-reset command was guessed.
- RED/GREEN fake-bus ASan/UBSan covered identity, config, short/error reads, negative axes, invalid frames and timing/wrap. Actual LVGL 8.3.10 checked all ten circle-safe status labels and unchanged counter/input behavior. Thirty-four Python regressions passed; independent review passed. S3 compiled 605,811 application bytes / 126,488 globals; exported app 605,952 bytes, valid chip 9/checksum/hash. Display/touch drivers retain the four exact DMA corrections; bootloader/partitions/OTA initialization are byte-identical to fallback.
- Re-identified the original S3/16MB target and verified preserved originals, exact sources/artifacts and unowned port. Application-only native-USB write at 0x10000 passed data-hash verification. Startup/continuing heartbeats identify v3-bringup-0.1.1 with UI ready and expander/touch ACKs. Actual physical sensor 0x6b returned WHO_AM_I 0x05 and revision 0x7c; config readback passed. Four fresh physical-bus samples had 1.0029–1.0040g magnitude and no reported sampling read errors. The gyro remains uncalibrated/biased; fitted A/C suffix is not established. No synthetic on-device inputs/samples were injected.
- Controlled still-versus-moving readings, native-axis interpretation, status-line physical readability, PSRAM/soak and actual shake/rest/cooldown/card behavior remain open. Reopening native USB can reset the board/counter; monitoring was not claimed reset-free. Exact private source/build/upload/runtime evidence is under `build/v3-s3-bringup/imu/`; previous `minimal/` evidence and full original images remain intact.

## Earlier V3 minimal display/touch installation — v3-bringup-0.1.0

- Photo-confirmed original ESP32-S3-Touch-LCD-1.85 Rev2.1; freshly identified the same S3/16MB backup target and unowned native-USB port before writing. Both full 16,777,216-byte originals remained unchanged and private.
- Added an S3-only minimal dark round display/touch test with a 160x64 Tap target, click-only count, version marker and five-second serial heartbeat. Audio, microphone, SD, Wi-Fi, speech, IMU and persistence are not initialized. No synthetic device taps were injected.
- Initial review blocked unsafe asynchronous color-bar buffer reuse/free; normal vendor LVGL flush also returned before color completion. A narrowly derived display adapter removes the startup call and drains queued DMA before returning its LVGL buffer. Exact prepared-source and actual-function queued-DMA tests went RED then GREEN; independent re-review passed. Original vendor archive/extraction, panel command/reset tables, other eleven drivers and bundled LVGL remain unchanged.
- Thirty-two Python tests passed; actual LVGL 8.3.10 with ASan/UBSan verified synthetic pointer hold/release, 100 clicks, cancellation, circle/focus bounds, rendered copy and heartbeat rollover. S3 recompilation succeeded: 603,687 application bytes, 126,424 globals, 201,256 bytes remaining by static accounting; exported application 603,840 bytes. Actual S3 headers, checksums/hashes, two-slot non-speech partition layout and package/component bytes were verified.
- Native-USB upload verified bootloader, partitions, OTA initialization and application writes. Serial readback confirmed v3-bringup-0.1.0, ui_ready=1, count zero and increasing uptime; a controlled reset additionally captured startup and post-reset expander/touch ACKs both one, continuing heartbeats and no observed crash markers. No V1/V2 contact occurred. This establishes boot of the minimal core-3.3.2 image, not manufacturer-certified all-peripheral compatibility.
- Physical operator confirmation: the visible counter responds to the button; an additional **100 physical taps** were reported, and only tapping inside the button increments it. This passes the basic visible-counter/touch-target check. It does not establish full-screen calibration, hold/drag behavior, soak or all-content readability.
- A later USB-monitor connection unexpectedly produced `USB_UART_CHIP_RESET` and cleared the counter despite DTR/RTS being set false before opening. Its zero count was therefore inconclusive for earlier taps, not evidence of failed touch. The physical confirmation above comes from the on-screen check; the monitor was closed for that check.
- Hold/drag/display-edge mapping, runtime PSRAM, IMU, sustained power/memory and full TrailGuide/shake behavior remain unverified. No authenticated V3 OTA service exists. See V3-BRINGUP.md for source integrity, build/upload recipes, private evidence locations and recovery boundaries.

## Earlier V3 initial silicon/backup/build checkpoint

- Probed a separately attached native-USB S3: revision v0.2, 40 MHz crystal, 16 MB JEDEC flash and an embedded 8 MB PSRAM silicon feature report. Secure Boot/Flash Encryption were disabled. USB/chip identity is not proof of the exact physical display board or PCB revision; rear markings were unresolved at that checkpoint and were later resolved by the Rev2.1 PCB photograph.
- Two independent complete 16,777,216-byte original-flash reads matched by SHA-256 and byte-for-byte comparison. Their sizes, hashes, mode-600 permissions and private mode-700 directory were independently rechecked. Factory images, raw logs and device identifiers remain ignored/private. No flash erase/write or firmware installation was performed; a RAM read stub and S3-only resets were used.
- The manufacturer-linked original-model demo archive was retrieved and checksum-pinned; the inspected local application and package hashes/sizes were independently rechecked. The unmodified board-level LVGL example compiled with existing core 3.3.2 and bundled LVGL 8.3.10/audio 2.0.0, reporting 2,489,075 application bytes and 188,396 globals. This is outside the manufacturer's stated core range, so host build success is not runtime compatibility or display/touch/IMU acceptance.
- The reference is an all-peripheral demo, including microphone/audio, Wi-Fi scanning and speech recognition, not a minimal TrailGuide application. Its exported merged image omits the separate speech-model payload; its partition layout differs from the original image. No candidate was flashed. Detailed source, reference pin map, exact build command, preservation/recovery and remaining gates are in V3-BRINGUP.md.

## V2 native physical-window emergence — v2-0.1.8

- Replaced the rejected static-label fade with a coherent blue faceted face and complete 20px type words approaching with perspective/tilt/drift, then settling still. Total 6000ms includes600ms darkness. Controls remain fixed; same-card Reveal all, instant Guide/Back and complete-copy scroll access are preserved. No content/revision/order, sensor, recording or menu change.
- Actual LVGL 8.4 snapshots use one bounded 106,240-byte caller allocation for immutable RGB565 source/output, outside unchanged 48KiB LVGL heap. There are no per-frame application allocations; missing memory retains instant complete text. It is safe fallback, not evidence that the physical motion worked. Native sampling lacks browser blur/soft shadows.
- RED/GREEN covered timing, blue face, low-heap guard, final raster settling and actual diagnostic fallback. ASan/UBSan passed both modes/all eight, full UTF8/long copy, spatial face/word movement, exact poses, fixed controls, scrolling, pointer hold/release/drag, OTA, rollover and independent deletion/teardown. Recorded 422 caller allocations with none remaining,100-cycle heap stability and zero changed window pixels at the representative final handoff. Thirty-four Python tests passed; independent review found no source blocker.
- C6 compiled 1,427,064 program bytes / 113,372 globals; exported app 1,427,168 bytes, valid chip 13/checksum/hash, within slot. Partitions remained byte-identical to v7. One authenticated wireless transfer installed v2-0.1.8 at 0x10000 from 0x340000; exact version/OTA readiness and later increasing uptime were read back. Previous C6 app/recovery and credentials remain private. No C6 USB or V3 access occurred for this update.
- Actual C6 contiguous allocation under Wi-Fi load, frame time, physical feel/readability/touch and sustained operation remain separate gates. Host raster success is not hardware performance or human visual acceptance.

## Earlier V2 whole-answer emergence — v2-0.1.7

- Replaced the rejected typewriter effect with the complete answer emerging together from darkness, Magic-8-Ball-style. Text, UTF-8 bytes, wrapping and geometry stay fixed throughout; only whole-label opacity changes using integer smoothstep. Nominal timing remains a 160ms settling beat and 2800ms fade. Reveal all restores full opacity on the same card; Guide/Back remain instant. No sensor, new input shortcut, new mode, content or tracking is added.
- A missing-opacity API test and then a finish-control-deletion visibility assertion were observed failing before the corresponding implementation/fix. Actual-LVGL ASan/UBSan checks passed across both modes/all eight cards, fixed complete text/layout, monotonic opacity, rendered-pixel changes, timing, same-card skip, navigation/OTA cancellation and teardown. Hidden/midpoint/clear frames from the real LVGL software renderer were inspected; they are not physical screen photographs. The Python suite passed all twenty-eight tests; generated outputs remained current. Independent review found no blocking defects.
- C6 compilation succeeded with 1,416,202 application bytes and 113,348 bytes of globals. The C6 image/version/slot budget and absence of the temporary disconnect hook were checked; partitions are byte-identical to the preceding build. Credentials, binaries and raw logs remain private.
- The first authenticated wireless transfer dropped with a broken pipe. The previous v2-0.1.6 application remained running and OTA-ready; no installation was claimed. A normal retry installed v2-0.1.7, switching from 0x10000 to 0x340000. Expected running version, continuing OTA readiness, changed slot and subsequent increasing uptime were read back. No USB upload or serial reset was used for V2; the transfer failure's cause was not established.
- Physical pacing/readability/touch acceptance remains open. The V3 direction now explicitly calls for whole-answer emergence; its older browser artifact remains a labeled legacy character-reveal timing/input study, not the intended finished visual effect.

## Earlier V2 gradual main-card reveal — v2-0.1.6

- Fresh Ask/Try entry and Another now use a 160ms settling beat followed by a UTF-8-safe progressive main-copy reveal, nominally complete at 2960ms with regularly serviced LVGL timers. The existing Another position temporarily shows Reveal all; activating it completes the same card without advancing the model and restores Another. Guide remains immediately available; Guide/Back and ordinary rerenders show complete text without replaying a draw. No main-text tap shortcut, motion input, new menu or setting is added.
- Navigation/Home, rerender, OTA start, external object deletion and owner teardown cancel the owned timer. OTA error dismissal preserves complete context. Canonical copy, card IDs/revisions/review states, fixed eight-card order, fonts and overflow access remain unchanged; V1, network/update code, credentials and round previews are untouched.
- Test-first regressions were observed before implementation across reveal stages, same-card skip, cancellation/OTA and teardown, plus UTF-8 helper coverage. Actual bundled-LVGL ASan/UBSan tests passed for both modes/all eight cards, timing, complete-copy scrolling and button/caption bounds. The 60,000-byte synthetic UTF-8 case tests prefix calculation, not a full label exceeding the configured 48KB LVGL heap. All twenty-eight Python tests, five V1/V2 C++ host suites and three generated-output freshness checks passed. Independent review found no blocking security or logic defects; physical pointer/hold behavior and reveal-specific tick-rollover coverage remain useful follow-ups.
- C6 compilation succeeded with 1,416,560 application bytes and 113,364 bytes of globals. Application image validation passed; the temporary disconnect bench hook is absent, partitions match the previous build byte-for-byte, and known-good recovery images remain private. Static sizes are not runtime heap measurements.
- Authenticated Wi-Fi OTA installed v2-0.1.6; the expected running version, continuing OTA readiness and active application slot 0x10000 were read back after transfer, changed from 0x340000. A subsequent read showed increasing uptime and the same version/slot. No USB upload, serial reset or repeated negative/interruption test was performed. The adapted USB selfTest compiled but was not rerun on-device during this update.
- Physical pacing, readability, real touch/drag/edge behavior, sustained runtime/network/power and learning outcomes remain separate human/hardware gates. See V2-TRIAL.md for the hands-on check; a successful wireless install is not evidence that the visual effect feels right.

## Earlier project-led mentoring and separate V3 reveal study

- Reframed the README, product purpose/P-19, delivery plan and handoff around meaningful projects, broad AI mentoring for individuals/teams and learner independence. Connected to AI Learning Lab's existing loop and flexible role map. Hardware remains an optional enjoyable companion, not the curriculum or a prerequisite for mentoring practice.
- Added the draft PROJECT-COACH.md learner-owned context brief and optional AI-coaching starter. These are usable text materials for review, not an integrated AI tutor, validated curriculum or measured learning outcome. Existing collected research remains a bounded review input; no private research workspace or participant material was published.
- Added optional P-20 and V3-EXPERIENCE.md for deliberate shake → one curated idea → gradual text reveal, including same-card Reveal all, Instant text, touch parity, cancellation and busy/context suppression. Actual V3 IMU thresholds, rest/cooldown, ordinary-handling rejection and display performance remain unverified.
- Built a separate portable prototype/v3-experience.html from the existing eight canonical draft pairs. Its simulated-shake and Another controls share shuffled mode-specific pools; gradual text is a curated reveal, not live AI generation. It stores nothing, reads no sensor and makes no network requests. The reduced study layout is not the full device/navigation implementation.
- All nineteen pre-existing Python tests passed before the new study was included; the full suite then passed all twenty-eight tests. The new nine-test suite uses actual system Chromium/CDP for fresh no-draw state, both modes/all eight identities, complete copy/overflow, shuffle boundaries, busy-input suppression, skip, instant/OS reduced motion, cancellation/stale callbacks, safe-circle targets and narrow-view panning, plus real pointer/keyboard/touch input. No browser script/console errors were reported by those checks. Generated-study freshness passed; a browser screenshot was inspected for visible copy/control clipping and simulator labels.
- Documentation links/headings and generic-public wording passed checks; P-01–P-20 are twenty unique IDs. Canonical content/revisions/review states, original full-flow round preview, V1/V2 source/configuration and protected AGENTS.md remain unchanged. No reset, upload or device connection was performed, and no new physical acceptance is implied. Human review of V2 wording/readability/touch remains pending.

## Earlier coaching direction and project handoff

- Added COACHING-DIRECTION.md and HANDOFF.md to capture facilitator development alongside team support, appropriate AI reliance, voluntary/access-aware participation and a proposed before/during/after coaching pattern. Research findings, practice materials, a pilot and additional interfaces remain pending; endorsement of direction is not evidence of effectiveness.
- Added planned P-19 without renumbering prior requirements or changing the existing core release scope. README, PLAN.md and the legacy Pi setup handoff now point to the current documents.
- Handoff covers the v2-0.1.5 implementation baseline, V1/V2/V3 boundaries, canonical content, exact build/test/OTA entry points, private configuration/recovery, actual evidence and remaining acceptance. Scheduled research is not presented as completed work.
- Documentation checks passed for generic wording, relative file/section links, nineteen unique requirement IDs and preserved core scope. All nineteen existing Python tests and generated-content/round-preview freshness checks passed. No application code, canonical content, firmware, private credentials or hardware state was changed by this documentation task.

## Earlier V2 conversational cue flow — v2-0.1.5

- Observation bodies now flow directly from What to listen for / What to notice instead of repeating Listen for / Look for / Notice. Only a matching leading instruction is removed; the remaining cue, cautions and negations are preserved, with sentence casing that retains mixed-case names and acronyms.
- You could ask and If helpful, offer replace formal follow-up/invitation captions. Main questions/actions, activities, title/Back context, canonical JSON, card IDs, revisions and draft review states remain unchanged.
- The new regression first failed for the missing formatter, then passed across all eight pairs. All nineteen Python tests and real-LVGL ASan/UBSan navigation/complete-copy scrolling checks passed. Independent code review found no blocking security or logic defects and confirmed all sixteen generated guides preserve the intended meaning.
- v2-0.1.5 compiled with 1,415,014 application bytes and 113,348 globals, preserving the existing OTA partitions/configuration. It was installed over Wi-Fi; running version, continued OTA readiness and active slot 0x340000 were read back remotely. No USB upload, serial reset or V1 change was used.
- Human review of the revised conversational wording remains separate. The older round browser artifact is unchanged and still awaits its own guidance-order alignment.

## Earlier V2 authenticated Wi-Fi/OTA — v2-0.1.4

- Optional station Wi-Fi and authenticated ArduinoOTA are implemented independently for C6, with a unique hostname/password, background connection/retry, an updating overlay and a read-only version/status endpoint. Credentials, raw logs and firmware binaries are local/ignored; public examples contain placeholders only. V1 was not reset, flashed or modified.
- The v2-0.1.2 USB bootstrap preserved the verified 8MB two-slot layout. A deliberate bench Wi-Fi disconnect left synthetic Ask/Try/Guide navigation working; automatic retry restored OTA and remote status. The final images omit the temporary disconnect hook.
- Incorrect-password rejection and an authenticated transfer interrupted after at least 10% were hardware-tested. The old application/version/slot remained running and OTA/status recovered. No automatic rollback or interrupted-power safety is claimed.
- Two authenticated network updates installed v2-0.1.3 at 0x340000, then v2-0.1.4 at 0x10000; running versions, active slots and continuing OTA readiness were read back over the LAN. A fixed Pi callback port initially hit TIME_WAIT before the second transfer; available ephemeral callback selection fixed it and passed a regression test.
- The final compiled application uses 1,415,126 bytes and 113,348 globals. Eighteen Python tests, V2 network/disabled-config tests, actual-LVGL ASan/UBSan checks (including update-overlay input suppression/context preservation) and both V1 C++ suites passed. Missing-formatter, overlay, image-validation, callback-selection and malformed-status tests were observed failing before the corresponding implementations/fixes.
- USB is no longer needed for the verified network update path, but the device still needs power and reachable trusted local Wi-Fi. Battery endurance, prolonged network soak, physical updating-screen review and retained future Team Trail data are separate acceptance gates. Internet-exposed, TLS-encrypted or signed-firmware updating is not implemented.
- Operating/build/security/recovery details: [V2-WIFI-UPDATES.md](V2-WIFI-UPDATES.md). Original factory backups, pre-network images and the known-working bootstrap remain private and preserved.

## Earlier V2 guidance refinement — v2-0.1.1

- Hands-on review identified redundant repetition at the start of Guide me. Ask now opens with What to listen for and follow-ups, without repeating its main question. Try opens with How to run it, then What to notice and follow-ups; a distinct suggested invitation is last, and exact action/activity duplicates are omitted. Card title and Back context remain unchanged.
- Regression tests first failed for the missing progressive formatter, then passed across all eight pairs, including per-card generated-guide checks and duplicate-invitation suppression. All fourteen Python tests and the actual-LVGL ASan/UBSan navigation/scrolling suite passed. Canonical card text, IDs, revisions and review states are unchanged.
- v2-0.1.1 compiled with 615,004 application bytes and 80,612 globals. C6-only USB upload verified four image hashes. Serial startup confirmed v2-0.1.1, Home and 364,092 free-heap bytes; the synthetic on-device diagnostic passed and returned to Home. V1 and the two-slot partition layout were unchanged.
- Updated physical readability/touch acceptance and independent-power/soak results remain open. A disconnected USB interval was not treated as a failed upload or as battery acceptance. The older round browser artifact remains unchanged and needs its guidance-order alignment separately.

## Earlier V2 first read-only TrailGuide application

- Basic manufacturer-demo operation was human-reported as working before replacement. No detailed edge/soak acceptance is inferred from that report.
- v2-0.1.0 DRAFT is compiled, C6-only USB-flashed with verified hashes and serial-verified. It opens Home and provides Ask/Try, Guide me, vertical full-copy scrolling and ordered Another using eight canonical draft pairs. No editorial state/revision was changed.
- Actual LVGL host tests passed with ASan/UBSan across both modes, all eight cards and sixteen guidance faces, button bounds and scrolling. Pure model tests passed 1,000 advances without adjacent repeats and preserved card context across Guide/Back. New generator tests first failed for missing implementation, then passed. All thirteen Python tests, both V1 C++ suites and generated-content/round-preview freshness checks passed.
- The on-device synthetic diagnostic passed both modes, all eight cards/sixteen guidance faces, complete-copy comparisons, bounds, Back and Another, returning to Home. Startup measured 364,092 free-heap bytes; static compilation reported 615,100 application bytes and 80,612 globals. Neither is a sustained memory/soak test.
- Human TrailGuide readability, actual touch/drag/edge behavior, reset/independent power and soak remain open. Theme filtering, linked-face shortcuts, team selection, recording/saving, Team Trail and authenticated OTA remain unimplemented. This is not production release acceptance.
- Independent review of the full code diff found no blocking security or logic defects. Non-blocking follow-ups are stronger host-object cache invalidation and per-record generated-content assertions; existing real-LVGL, generator and on-device checks passed as described above.
- Source/build/review details: [V2-TRIAL.md](V2-TRIAL.md). V1 was untouched. Original backups and working demo binaries remain private/ignored; the 8MB two-slot layout is unchanged from the verified demo.

## Earlier V2 C6 first USB bring-up

- C6 silicon revision 0.2 and 8MB flash were probed. Secure boot and flash encryption were reported disabled. The original full flash was preserved in two matching 8,388,608-byte reads before any write.
- The correct manufacturer LVGL 8.4.0 display/touch example was built with Arduino core 3.3.2 and bundled GFX 1.5.9, uploaded only to C6, and hash-verified. A controlled C6 reset produced the example banner, LVGL version, touch-driver initialization data and `Setup done` over serial.
- Physical display/readability, touch mapping, exact PCB identification and runtime performance are not yet accepted. The temporary demo has no TrailGuide content, storage or OTA implementation; reserved OTA slots alone are not functional OTA.
- V1 was not reset, flashed or opened for serial monitoring during this work. Details, package-label caveat, source checksum, pins, partitions, reproducible build and private-image recovery procedure are in [V2-BRINGUP.md](V2-BRINGUP.md).
- This checkpoint supersedes the pending probe/backup/demo items in the earlier designation-only record below; it does not turn that earlier documentation update into a hardware test.

## Earlier target designation update

- V1 remains the classic ESP32/SH1106 working fallback.
- V2 is now the supplied ESP32-C6 1.47-inch 172x320 portrait capacitive-touch board with pin headers. V3 is the future round ESP32-S3-Touch-LCD-1.85 target.
- USB inspection sees an Espressif USB JTAG/serial interface at /dev/ttyACM0 and the original CP2102 interface at /dev/ttyUSB0. These are enumeration observations, not C6 chip/revision/flash probes or display/touch verification. Re-identify both before any upload; ports may change.
- The family description matches manufacturer documentation, but exact PCB/revision, drivers, flash/partitions and original-firmware recovery remain pending. No reset, serial probing or flashing was performed for this update.
- V2-SPEC.md now defines the portrait target; the prior round contract is preserved in V3-SPEC.md. Existing circular HTML/content/tests are unchanged and remain the V3/shared-flow preview. Earlier records using V2 for the round board are superseded by this designation.
- Documentation checks passed: generic wording, local links, eighteen stable requirement IDs and correct V2/V3 target mapping. The unchanged round preview passed its freshness check and all eleven existing Python tests passed. Source, content and device firmware are unchanged; this is not C6 build/display/touch acceptance.
- Next V2 work is safe identification/recovery preparation, correct manufacturer display/touch bring-up and a separate portrait UI adaptation. Do not shrink the circular coordinates or copy binaries across targets.

## Current checkpoint

- V1 hardware previously identified as ESP32-D0WD-V3 revision 3.1, 40 MHz crystal, 4 MB flash, CP2102 USB UART. Re-identify the actual board/port before flashing.
- Firmware v1-0.2.4 uses the actual Hosyond 1.3-inch SH1106 128x64 white OLED, SDA21/SCL22 at 3V3. The earlier SSD1306 driver assumption was incorrect.
- v1-0.2.4 was ESP32-compiled, USB-flashed and serial-verified: correct version, SH1106 at 0x3C and OTA readiness.
- Human-observed checks confirm readable sampled cards, one new card per BOOT press, no repeated advance while held, and fifty mechanical presses. Full-deck readability, thirty-minute soak result and independent-power acceptance remain unconfirmed.
- Host suites passed with C++17 and warnings as errors. These do not verify physical pixels or power behavior.
- The expanded content catalog is authored as forty draft cards across eight themes, with structured JSON and a readable catalog. Content tests pass; facilitator review and hardware layout checks remain pending.
- The first read-only V2 TrailGuide trial is now implemented; production theme-filter behavior, real team storage and Team Trail remain unimplemented. The round browser preview is retained for V3/shared design. Neither artifact stores interventions. V1 source and flashed firmware are unchanged. See [V2-TRIAL.md](V2-TRIAL.md), [PROTOTYPE-REVIEW.md](PROTOTYPE-REVIEW.md), [PRODUCT-SPEC.md](PRODUCT-SPEC.md) and [PLAN.md](PLAN.md).

## Verified V1 toolchain

- Arduino CLI 1.3.1.
- Espressif Arduino core 3.3.2.
- Adafruit SH110X 2.1.14, GFX 1.13.0 and BusIO 1.17.4.
- Python esptool 4.11.0.
- Target: esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default.
- Default 4 MB partition layout includes OTA metadata and two 0x140000-byte application slots at 0x10000 and 0x150000.

Install BusIO, GFX and SH110X in dependency order with pinned versions and --no-deps to avoid unintentional upgrades. A slow core download previously required `arduino-cli config set network.connection_timeout 600s`.

## Build and upload

Run from the repository root. Replace PORT with the actual detected serial port. These commands describe the procedure; rerunning them requires checking hardware and preserving compatible credentials/partitions.

```sh
arduino-cli board list
arduino-cli compile --jobs 4 --fqbn esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default --output-dir build/v1-0.2.4-sh1106 oled_oracle
arduino-cli upload --fqbn esp32:esp32:esp32:FlashSize=4M,PartitionScheme=default --port PORT --input-dir build/v1-0.2.4-sh1106 oled_oracle
python3 tools/ota_upload.py ESP32_ADDRESS build/v1-0.2.4-sh1106/oled_oracle.ino.bin
```

The verified SH1106 build used 995,259 application bytes and 50,932 bytes globals. Compiled outputs contain credentials and must remain local/ignored. Wi-Fi/OTA secrets belong only in ignored, mode-600 oled_oracle/secrets.h. See [WIFI-UPDATES.md](WIFI-UPDATES.md).

## Host verification

```sh
g++ -std=c++17 -Wall -Wextra -Werror -I tests/stubs tests/firmware_test.cpp -o "${TMPDIR:-/tmp}/trailguide-firmware-test"
"${TMPDIR:-/tmp}/trailguide-firmware-test"
g++ -std=c++17 -Wall -Wextra -Werror -I tests/network -I tests/stubs tests/ota_test.cpp -o "${TMPDIR:-/tmp}/trailguide-ota-test"
"${TMPDIR:-/tmp}/trailguide-ota-test"
```

Passing coverage: missing OLED/allocation failure, both display addresses, 1,000 deck cycles, debounce/hold/rollover, layout limits, offline advice, network retries, OTA listener reconnect and callbacks. The SH1106 regression assertion failed against the earlier SSD1306 implementation and passed after correction.

## Hardware verification evidence

- SH1106 USB upload hashes verified; serial startup confirmed v1-0.2.4, display address and OTA readiness. Human display/button observations are summarized above.
- Earlier serial-only builds verified fifty electrical GPIO0 cycles through the UART bridge with exactly fifty cards, no adjacent repeats or resets, and one draw while held. Electrical cycles are separate from mechanical-button checks.
- Wrong OTA password was rejected while normal firmware remained usable.
- Two consecutive direct-IP wireless updates succeeded from v1-0.2.1 through v1-0.2.2 to v1-0.2.3, with serial version/readiness confirmation.
- A sender-interrupted authenticated transfer at 10% reported OTA error 4. Normal firmware remained bootable and a later upload succeeded. This verifies interrupted-transfer recovery, not automatic rollback or power-loss safety.
- A temporary bench build disconnected Wi-Fi once after 20 seconds. Advice continued offline; production retry restored Wi-Fi/OTA. Normal v1-0.2.3 was restored afterward, before the SH1106 correction.
- OTA code/credentials are unchanged in v1-0.2.4. Prior OTA tests are not a claim of newly repeated OTA tests on every build.
- USB errors and an initially garbled serial read were observed; causes were not established. A controlled RTS reset with DTR deasserted produced readable 115200 8N1 startup. I2C acknowledgement alone does not verify visible pixels.

## Recovery

Keep local ignored build directories and source snapshots. Known SH1106 build: build/v1-0.2.4-sh1106. Earlier verified normal build: build/v1-0.2.3; it predates the corrected display driver.

Previously recorded application SHA256 for build/v1-0.2.3:
`3564e24ea0741ed7df65ff36fac86f9a61b56465bd5ee1afb78da1eceb43543c`.

Original flash backup: build/recovery/pre-trailguide-4mb.bin. Previously recorded SHA256:
`d911e73b462b36998a21300ed3be4423e6cf3fc7ba0052f400828e9712bfbc5e`.

Use USB recovery with the known build's original partition settings; prefer the SH1106 build for this OLED. Do not send merged images or partition-table changes over OTA. Private verification logs remain under ignored build/verification/ and are not repository documentation.

## Remaining acceptance

- Confirm every card's readability, independently of click count.
- Obtain the thirty-minute continuous-operation result; do not infer it from a timer/reminder.
- Verify reset, power-cycle, USB reconnect and USB-independent power operation.
- Router/AP disappearance and return, mDNS upload, and power loss during transfer remain untested. Direct-IP OTA is verified; no automatic rollback is claimed.
- Inspect V2 hardware before implementing its drivers or persistence layer.

## Content expansion validation

The content contract test was run before the expanded catalog existed and failed for the expected missing-catalog condition. After adding structured JSON and its readable Markdown view, it passed: forty unique card IDs/titles, eight themes with five cards each, required guidance fields, recognized situation tags, and verbatim preservation of the twelve original V1 actions. Both C++17 host suites were rebuilt with warnings as errors and passed. No firmware source was changed and no device was reset or flashed. All forty entries remain editorial drafts, not facilitator-approved or hardware-layout-verified content.

## Agent workflow content validation

Ten existing cards now carry draft CLI-agent variants with bounded prompts, human checks and separate Codex/Claude Code notes. The base actions, stable IDs, eight themes and forty-card count are unchanged. The ten adapted entries advanced to revision 2. A cross-theme collection and readable guide are authored; collection UI, variant tracking and tool-note display are not implemented.

The new content test first failed for the expected missing-collection condition, then passed after adding metadata and synchronized catalog/guide content. Official project-instruction documentation was inspected; guidance asks facilitators to verify installed versions, settings and active instructions rather than assuming identical tool behavior. No firmware was changed, reset or flashed.

## Round UI and two-mode specification refinement

Refined the documentation contract with a single Guide me entry, Asked this / Offered this move labels, the existing `question` field name, explicit proposed draft/reviewed/retired scopes and eligibility on draws/linked faces/saves/history reuse. Recorded historical snapshots stay distinguishable from eligible content for new use.

Added ROUND-UI-SPEC.md with proposed circle-safe coordinates, typography/touch budgets, three-choice paged menus, overflow behavior, explicit return context and an eight-card read-only prototype plan. All thirteen proposed rectangle envelopes were mathematically checked inside a radius-168 circle on an assumed 360x360 canvas. This is geometry evidence only: no font rendering, touch accuracy, readability, runtime performance or hardware clearance was established. Proposed thresholds and dimensions remain review inputs.

A separate read-only UX critique supported short pages, one help entry, mode-specific recording and eight representative pairs. It also prompted explicit Home-without-auto-draw behavior and a circular input mask, not just visual clipping. The critique did not run geometry or hardware tests. Independent geometry checking and existing-suite results are reported separately.

Final documentation checks passed: thirteen circle-contained rectangle envelopes, seventeen unique requirement IDs, parseable illustrative JSON, relative links, generic public wording and documentation-only scope. Both Python content tests and both rebuilt C++17 suites passed. None of these checks establishes actual font rendering, touch accuracy or new runtime behavior.

During the preceding specification-only step, no content JSON, firmware or storage schema was changed and no prototype was built. The subsequent prototype build is recorded below. No device was reset/flashed; full catalog enrichment, production eligibility/pools and mode-aware storage still follow facilitator review.

## Initial read-only prototype build and QA

Eight representative cards now carry additive draft Ask/Notice/Follow-up and Try invitation/Notice/Follow-up fields. Each affected revision advanced once; base wording, all forty stable IDs, eight themes and existing agent variants are unchanged. The content test first failed for missing paired fields, then passed after authoring and catalog synchronization. All scopes remain draft; structural validation is not editorial approval.

A portable prototype/template and standard-library builder now exist. The initial parent verification ran eight Python tests, including actual Chromium navigation, canonical-copy fit and pointer safeguards; they passed. Both existing C++17 suites also passed, and V1 firmware remained unchanged. The parent separately opened the actual file in a browser and exercised Ask, Guide me, paging, Back and disabled tracking controls with real pointer clicks. This is browser evidence only, not facilitator or hardware acceptance.

The initial review found three regression issues: reopening Team inside its picker overwrote return context; a 320-pixel viewport clipped the left of the fixed-size circle; duplicate theme metadata passed the unique-theme count check. A separate fix context added failing regressions and corrected all three. The parent reran all eleven Python tests successfully and independently rechecked double-open/Back and actual 320-pixel layout in the browser: card/mode/draw count were preserved and the circle started at x=0 in a horizontally scrollable region while remaining 360 pixels wide. Browser-helper screenshots were intermittently unavailable, but that does not replace functional checks or justify claiming a screen was inspected when it was not.

The independent full-code review found no security concerns and one blocking outside-and-back mouse-drag defect. A separate fix added a real Chromium CDP regression: it failed before the fix with draw count 2 instead of 1, then passed after leaving the dial canceled both pending press/release. Normal mouse taps and Enter-key activation also passed. The parent reran all eleven Python tests successfully after this correction. A separate narrowly scoped re-review approved the pointerleave fix with no security or logic findings; it did not independently rerun tests. The parent also reran the builder freshness check and both C++17 suites. Arbitrary custom theme-label width remains a review consideration; the canonical labels are covered by browser fit checks.

Implementation adjustments for this review artifact: compact menu context sits at y=111 rather than the initial y=116; the full title has a separate Guide page; overflow uses an explicit Full text: Guide me route rather than scroll-only access. Review these choices with the facilitator before treating them as final interaction decisions. Recording, saving and history remain unavailable, and synthetic fixtures are not real cards or team records.

## Documentation validation

The product specification defines stable requirement IDs; the current document includes P-01 through P-18. Related document links and whitespace checks passed when the specification was added, alongside both rebuilt host suites. Planned V2 behavior is not verified by those V1 tests. Keep generic public docs separate from private operational context and preserve Git history unless a rewrite is explicitly authorized.

## Two-mode facilitation specification

Added FACILITATOR-MODES.md with Ask the Team / Try a Move, core Guide me support, paired-content authoring, navigation rules, mode-aware saves/use events, legacy migration, acceptance checks and a development-host implementation sequence. Added stable requirement P-16 and reconciled home navigation, core scope, delivery sequencing and V2 requirements. README links to the handoff.

GitHub readback verified the exact published contents of the handoff, product spec, plan, V2 spec and README. This is documentation verification only. No firmware, content JSON, test code or device state was changed; no host suites, compile, upload or hardware checks were run for this documentation update. Paired facilitator content, prototype, mode-aware storage and V2 implementation remain pending.
