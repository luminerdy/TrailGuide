# Delivery A — candidate22 status

## Identities and scope

- **Installed and physically accepted baseline:** `v3-0.1.21-global-gestures` DRAFT, application **691312 bytes**, remains running. Its qualitative global-gesture acceptance in Classic and Coaching is preserved; it is not exhaustive handling or field-reliability proof.
- **Source/build candidate in this snapshot:** `v3-0.1.22-reliability` DRAFT, application **691408 bytes**. It is **not installed**. Delivery A covers verification, runtime and tooling hardening only. Source publication does not transfer firmware or establish physical acceptance.
- **Accepted-source checkpoint:** unchanged installed21 production, the reviews and the update plan were published separately at commit `3b79573`. This candidate builds on that checkpoint; accepted21 remains the device and recovery baseline.

This is the single candidate22 gate ledger for the README, both flow maps, handoff, plan and project/product/V3 specifications and trial guide. Their installed21 and older evidence remains version-specific; it is not candidate22 evidence. The [update plan](UPDATE-PLAN-v3-0.1.21.md) and its [finding tracker](reviews/UPDATE-PLAN-TRACEABILITY-v3-0.1.21.csv) retain their own scope and historical baseline statements.

## Candidate changes, not a new experience

- **Runtime:** known sensor read/raw/clipping faults cancel partial gesture intent while retaining NotReady and bounded-pause behavior. Malformed recorder command frames are discarded through newline, including NUL, DEL, non-ASCII and invalid CR, without changing retained recorder state. Owner-side pending-cue validation invalidates a published but unaccepted stale cue without resetting its sequence or stopping already-playing audio during navigation; sticky OFF cancellation remains intact.
- **Verification/tooling:** pinned presentation-region mutation checks replace the whole-header hash boundary; owned helper-group waits/cleanup are bounded, with explicit attempt reports, final inventories and incomplete-attempt detection. Fresh C++ producer-bound wire fixtures must be integrated, not silently skipped when missing/stale. Delivered HTML is checked with `--check`, not regenerated. Preparation uses anchored paths without intermediate symlinks, staged output and manifest-last publication with rollback; this is **not atomic multi-directory publication**. Native revision/control validation retains uint32 bounds without changing browser Unicode policy; port-descriptor error paths clean up ownership.
- **Unchanged:** accepted appearance, sensing policy, global intentional flip/shake, pointer/scroll suppression, one draw per gesture, six-second whole-face reveal plus five-second reading hold, no extra timed black pause, and Classic boot default. Pending-cue hardening changes neither appearance nor startup defaults. UI Instant sharing and Classic scroll-position work remain deferred, not implemented.
- **Content/roles:** 40 canonical DRAFT cards, 12 Classic pairs, 55 Coaching entries and 11 canonical references are unchanged. C1 wording drafts and C2 optional quick reference await approval and are **not applied**. The six facilitator-chosen paths are memory cues, not diagnosis, scores or a curriculum. Preserve **Define → Direct → Check → Adjust**, human/AI/hybrid choice, requested direct help and unrestricted passing. Check AI work against real evidence and compare end-to-end effort, not generation speed alone. The device remains optional.

## Completed evidence and its limits

| Completed scoped evidence | What it establishes |
| --- | --- |
| 76 host composition cases using actual LVGL, IMU-driver, sketch, UI and audio-adapter code with synthetic inputs under ASan/UBSan, including 40 synthetic raw-fault cases through the real UI; independent runtime review of six files and three new tests without blockers | Scoped runtime regression evidence, not physical acceptance. |
| Full canonical native run: all 147 child commands passed; 533 archived framebuffers were hash/readback verified and byte-identical to the corresponding accepted21 captures | Current-source host integration and captured-presentation regression evidence, not physical display, motion or sound acceptance. |
| Full isolated Python/browser run: 125 tests passed, with zero failures, errors or skips; owned process cleanup, all protected installed inventories and existing global browser-profile bytes verified unchanged | Required current-source Python/integration/browser checks completed. Test dependencies were deduplicated only after successful real preparation and full pinned-byte verification; synthetic negative fixtures were not shared. |
| 44 focused Python checks, zero errors/skips | Earlier focused tooling/contract evidence; overlaps the completed full suite and is not added to its count. |
| Tooling blocker fixes and independent unblock review: five probes and 27 tests | Nested owned-session cleanup and rollback-marker failure/backup retention regressions passed. These counts are separate, potentially overlapping evidence, not an additive suite total. |
| Independent final integration and offline package review passed for the exact candidate image and retained ELF | Software-candidate approval only. Deployment, runtime-readback and restoration helpers require separate preparation/review before an authorized handoff. |
| Actual S3 candidate build/package, 17 source hashes and compatible bootloader/partitions; all 15 accepted installed inventories exact | Build/package and preservation evidence only; no candidate22 transfer or running-target verification. |

The **424 C objects from accepted v3-0.1.16** are reused read-only after source, library/cache, configuration and flag provenance checks. This is **not fresh C compilation**. The completed canonical candidate22 job freshly compiled current application/test C++, including the three new runtime tests.

## Pending or unresolved — no release-ready claim

- The completed browser run left existing global browser-profile bytes unchanged and no new global scoped-profile directory. The nonblocking TMPDIR-outside-sandbox and pre-run setup-report limitations still apply; this single run is not a universal cleanup guarantee.
- Candidate22 installation, exact running-target readback, physical rendering/audio/handling checks and sustained field reliability are not established.
- The original intermittent turnover-stuck-compass/blank root cause remains **open**. These fixes and scoped passes neither diagnose it nor establish its repair.
- The code/package reviews and software checks above concern this source snapshot. They are not installation, live runtime verification, physical acceptance or promotion out of DRAFT.
