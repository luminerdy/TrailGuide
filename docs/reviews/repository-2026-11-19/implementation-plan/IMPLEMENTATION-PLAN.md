# TrailGuide implementation plan

Status: **Proposed; implementation has not started.**
Baseline: public commit `2c8a05881495c9cc8edc8f8d7bb020f77141c0e6`, current source `v3-0.1.31-mode-switch`.
Input: the November 19, 2026 critical review and its 23 distinct findings.

## Outcome and recommended first package

Make TrailGuide easier to understand, check and contribute to without destabilizing the working devices or expanding its purpose.

**Start with Phases 1–3:** a reproducible public check tier, clearer repository documentation and a current requirement/evidence index. These are off-device changes. Publish them after their checks pass, if that implementation/publication scope is approved.

Content revisions, runtime changes, protected-file edits, optional release creation and hardware operations have separate gates below. Creating this plan does not authorize them.

Use the same loop for each package: **Define → Direct → Check → Adjust**. Define the scope and acceptance checks first; implement only that scope; run checks and a bounded review; fix supported findings and stop when the package meets its acceptance criteria. Do not turn one cleanup into an endless audit or broad refactor.

## Preserve throughout

- Keep the frozen28 tag, `content/v3-baseline.json`, private evidence, backups and credentials unchanged.
- Keep both installed31 units running. No device enumeration, serial opening, reset, recording, flashing or debug access during Phases 1–3.
- Leave `AGENTS.md` untouched unless its protected-file boundary is explicitly changed by the user. Do not route around that protection.
- Preserve current canonical content, draft review states, approved appearance, full-copy access, timing, independent Classic Ask/Try switching and current startup default until a specifically approved increment changes them.
- Keep current source identity separate from candidate source, installed payloads and physical acceptance. Do not weaken a source checker to make an unbuilt or uninstalled candidate look like installed31.
- Keep participant identity, transcripts, private captures, credentials and firmware binaries out of public changes.

## Phase 1 — Public verification foundation

**Findings:** D-01/A06, D-02, D-03, D-04.
**Effort:** medium.
**Dependency:** none; establish this before publishing later code changes.

### Deliverables

1. A proposed `tools/check_public.py` entrypoint with the documented command `python3 tools/check_public.py`.
2. A public-core test selection that uses only tracked inputs, plus a fresh C++ recorder producer and required protocol integration checks.
3. A separate document-generation smoke job with a declared environment. Browser, SDK, vendor-dependent and private historical-equivalence suites remain explicit separate tiers.
4. A proposed `.github/workflows/public-checks.yml`, with reviewed pinned action revisions, read-only repository permissions, no secrets and safe handling of untrusted pull requests. No device or OTA operations.
5. A proposed `docs/DEVELOPMENT.md` and document dependency file, recording supported Python/compiler/OS versions, ReportLab, fonts and `pdftotext`. Resolve exact compatible pins during implementation; do not assume latest versions work.
6. Integrity/preflight hardening in `tools/v3_native_objects.py` and `tools/build_coaching_display_guide.py`, with focused regressions under `tests/`.

### Implementation

- Use an explicit, reviewed case inventory rather than silently excluding tests after failures. Report unique case IDs, failures, skips and unavailable tiers.
- Fail on zero discovered tests, failed child commands, unexpected missing cases or skipped required integration cases. Preserve child failure exits.
- Compile and execute the recorder producer before the eleven integration cases; reject missing or stale producer artifacts.
- Replace release-critical assertion-only integrity checks with explicit rejection. Leave the already-explicit source-inventory checks strict.
- Check document prerequisites before writing published outputs. Generate into an owned staging directory, verify content/layout, then publish with failure recovery that preserves previous documents. Do not claim multi-file power-loss atomicity.
- Keep private historical fixture requirements disclosed; do not upload those fixtures, relax assertions or present the public tier as the full native suite.

### Acceptance

- The documented command succeeds from a clean public-source checkout without `build/`, the vendor ZIP, SDK, credentials or private installed records.
- A deliberately failing case and a deliberately missing required case both return failure; successful execution runs a nonzero expected set.
- All eleven producer-backed protocol cases run without skips.
- Corrupt source/configuration/object mutations reject under normal Python and `PYTHONOPTIMIZE=1`.
- Missing ReportLab/font/`pdftotext` and failed generation leave prior published guide files unchanged, with actionable errors.
- Document smoke checks preserve all 55 texts and 48 guidance bindings, continuous layout, prompt/guidance grouping and the approved readable typography. Compare extracted content and layout, not timestamp-dependent PDF bytes.
- A real GitHub run passes on the exact proposed commit. No CI-success claim before checking that run.

## Phase 2 — Reader-first documentation and onboarding

**Findings:** A01, A02, A03, A04, A09, B-05.
**Effort:** small to medium.
**Dependency:** may be drafted alongside Phase 1; links to new commands must not land before those commands exist.

### Deliverables

- Revised `README.md`: purpose first, then three short routes—use the mentoring materials, explore a labelled preview, or develop the current device.
- A proposed `docs/CURRENT-STATUS.md`: the single human-readable current checkpoint, linking the technical manifest, release evidence and open acceptance gates.
- Short context-appropriate status pointers in the affected specs/handoffs instead of thirteen copies of the same release paragraph.
- Targeted corrections in `docs/PROJECT-SPEC.md`, `docs/PRODUCT-SPEC.md` and other identified current-status passages. Preserve genuinely historical statements under explicit historical headings.
- Contributor guidance in the new development document or a small `CONTRIBUTING.md`.
- Correct historical-review links in `docs/reviews/README.md`.
- Prominent entry-link/companion-document notes describing historical preview behavior and omitted fields. No prototype renderer redesign in this phase.

### Acceptance

- Each of the three README routes reaches one useful artifact or an executable setup/check path without searching the work log.
- Current prose agrees on release identity and actual behavior; old20/28/11 statements are either corrected or explicitly historical, not globally replaced.
- Relative links and affected heading anchors resolve. Changed Mermaid blocks render; unchanged blocks are verified unchanged rather than needlessly regenerated.
- Current Coaching guide remains concise and source-matched; no return to per-entry navigation boilerplate or one-entry-per-page layout.
- The private/native prerequisites and unresolved Coaching shake acceptance remain visible.
- `AGENTS.md` remains byte-exact. **A02 cannot be marked fully closed** while its conflicting instructions remain: record the limitation and request explicit permission only if changing that file is wanted.
- B-05 closes only the preview-interpretation problem. A native-equivalent browser Guide remains separate scope.

## Phase 3 — Requirement traceability and verified publication

**Findings:** A05; optional A07.
**Effort:** medium for the index; small for optional metadata.
**Dependency:** Phases 1–2 provide stable check names and document routes.

### Deliverables

1. Proposed `docs/REQUIREMENTS-TRACEABILITY.md` and a small machine-readable backing file.
2. A validator checking stable P-ID coverage, referenced paths/checks and explicit unknown/deferred states.
3. Reviewed, scope-controlled commits and verified GitHub publication of the approved first package.

For each requirement, record target, implementation status, implementation location, named check, tested revision, evidence origin (fresh or reused), physical/user acceptance, open gaps and next action. Keep implementation and acceptance as separate fields. Include all stable P-IDs, marking future scope deferred rather than failed.

Link existing content/page coverage and historical remediation records; do not present them as substitutes for product acceptance. Public evidence summaries may identify retained private evidence without exposing machine paths, device IDs or raw captures.

### Acceptance and publication

- Every stable P-ID has an explicit disposition. Missing evidence reads unknown/unverified, not passed.
- A reader can distinguish source identity, tested behavior, installed state, qualitative feedback and future features without reconstructing the chronology.
- An independent scoped review checks the index against the cited source/checks; it does not demand inventing proof for untested claims.
- Stage only the approved paths, audit newly reachable public content, run affected checks, then perform a normal push. Read back the exact remote commit and changed file bytes.
- Frozen28 and installed31 source/payload evidence remain unchanged. The first package requires no new target build or installation.

**Optional, not included by default:** add About description/topics. A source-only current DRAFT tag/release requires an explicit opt-in; preserve28, identify the exact source commit and limits, and publish no private binaries. A hosted preview is optional and is not needed to finish this package.

## Phase 4 — Small content proposal and real-use feedback

**Findings:** B-01, B-02, B-03, B-04, A08.
**Effort:** small initial proposal; integration depends on the approved fields.
**Dependency:** does not block the already-approved real conversation trial.

### First increment

- Propose two or three activity-specific Guide replacements, starting with cases such as PROGRESS-E01 and UNCLEAR-E01. Show complete before/after fields, source IDs, rationale and consumer effects.
- Replace generic portions rather than append more warnings. Preserve passing, requested direct help, human-only options, uncertainty and consequence-sensitive checking.
- Offer explanation, demonstration and co-building examples without removing useful checking activities or adding another mandatory framework/context.
- Propose one optional success/checking field and a brief human/AI/hybrid example in `docs/PROJECT-COACH.md`.
- Consider OPEN-03/05/06 and the identified synthetic-only canonical fields only as later exact-wording proposals, not automatic edits.

### Acceptance and gates

- User approves exact canonical field changes before they are merged. Preserve IDs, shared-reference ownership, unrelated fields and draft states.
- Validate the resolved collection and guide-to-entry binding; keep differing guidance separate. Check complete copy in the actual native view before any device-readability claim.
- Canonical content changes alter runtime source: use a separately identified candidate/release and candidate documents. Do not silently regenerate the current31 guide from uninstalled new wording or change the installed31 manifest to fit it.
- A participant may choose, adapt or skip; requested direct help remains available. Capture only voluntary observations, not recordings, identities, scores or inferred learning outcomes.
- Use what helped/what got in the way to choose the next small change. No forced quota, exhaustive role-play or claim of proven efficacy.

The first conversation can proceed now on the unchanged31 touch interface. It need not wait for repository work or new firmware.

## Phase 5 — Narrow technical corrections

### 5A. Legacy V1/V2 Wi-Fi disable precedence

**Finding:** C1. **Effort:** small. **Dependency:** Phase 1 public test foundation.

Change only the relevant guards in `oled_oracle/ota_support.h` and `trailguide_v2/ota_support.h`, with regression fixtures. Test four configurations for each target: neither macro, configured, disabled, both. Disabled must prevent Wi-Fi/OTA calls even when configuration exists; configured-only behavior must still work. Include an auto-secrets-inclusion case without real credentials.

This is an off-device source/test correction. It does not authorize any V1/V2 or V3 update, and it is not a reason to touch the working V3 units.

### 5B. Current V3 diagnostic range consistency

**Finding:** C2. **Effort:** small code change; larger release-validation cost. **Gate:** separately approved V3 candidate scope.

Derive startup range telemetry from the driver constants instead of the stale literal, with a composed startup-log regression. Preserve sensor configuration and scaling; this is not a gesture retune.

Because this changes runtime source, create a new candidate identity and isolated source tree. Keep main's installed31 manifest truthful until the candidate's publication/deployment status is deliberately reconciled. Bind any new source manifest to the actual prepared/SDK sketch; do not edit hashes merely to waive a mismatch.

A future installation needs its own approved scope, target identity, preservation, fresh build/ELF/runtime mapping, review, full application readback and runtime verification. Old31 deployment approval is not permission for a new update. No deployment is part of the first package.

## Phase 6 — Conditional risk investigation

**Findings:** C3, C4, C5. **Effort:** bounded investigations; correction size unknown.

- **C3:** use the actual pinned LVGL harness to test Classic Home when object creation can succeed but the compass timer cannot be allocated. Measure navigation fragmentation. Confirm or dismiss the risk before proposing a minimal fallback change.
- **C4/C5:** start with retained evidence and off-device replay of eligibility, sample health/freshness, acquisition, recognition and dispatch. Establish what can be reconstructed for both detectors from a known initial state. Repair the host observer before proposing another human recording window.
- Only if that evidence is insufficient, propose a bounded, versioned shake-state diagnostic extension. Do not tune thresholds from speculation, repeat unrecorded shaking, or treat a CRC/fallback/manual success as automatic-shake acceptance.

**Resource gate:** remeasure storage and dependencies first. The review's external volume was absent and internal storage was low; do not assume an old build allowance still exists. No SDK/vendor/large native work until adequate space plus the required reserve is admitted. Keep every probe in an owned output root and retain failures.

A disproved risk can close with evidence and no code change. New firmware or recording requires a separate scope/coordination decision.

## Deliberately deferred

- **C6:** no broad Classic/Coaching controller refactor. Consider a small shared mechanism only when a demonstrated fix needs it and transition tests protect the mode-specific policies.
- Native-equivalent browser renderer, hosted preview and broader evidence research are separate optional work, not hidden prerequisites.
- No new networking, microphone, autonomous coaching, learner tracking, cloud integration, startup redesign or full-deck rewrite.

## Delivery and approval boundaries

Suggested landing units are public checks/integrity, reader-facing docs, and requirement traceability. Prepare docs in parallel if useful, but land command-dependent instructions with or after working code. Run one scoped independent review per package; after a fix, obtain a focused unblock review rather than restarting unrelated checks.

**Recommended first authorization:** Phases 1–3, including GitHub publication after checks. Exclude `AGENTS.md` edits, optional tag/release creation, canonical wording changes, runtime modifications and all device operations. Later phases remain held until their stated gates are met.

The attached finding map assigns every one of the 23 review findings to scheduled, restricted, proposal-first, optional, conditional or deferred work. A06 is already merged with D-01. Partial or deferred work must not be reported as fixed.

## Plan acceptance

- All 23 finding IDs mapped exactly once to a primary disposition, with no missing or invented IDs.
- Phase dependencies, file scope, deliverables, failure checks and stopping gates stated.
- Protected source and current GitHub baseline unchanged while producing this plan.
- No implementation, publication, participant contact or device operation performed by creating the plan.
