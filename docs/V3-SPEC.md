# V3 — Round Trail Guide Compass

Version assignment: this is the future round target. The current V2 is the portrait ESP32-C6 board described in [V2-SPEC.md](V2-SPEC.md). The existing round browser preview is retained for V3; renumbering does not verify new hardware or port the UI.

## Product requirements

The [living product specification](PRODUCT-SPEC.md) is the canonical source for the touchscreen experience: Ask/Try unfiltered draws, eight themes, team selection, deliberate mode-aware tracking, saved ideas and durable Team Trail history. Follow-through, favorites, situation shortcuts and expanded guidance are phased enhancements. See [PLAN.md](PLAN.md) for delivery gates. Ask the Team, Try a Move and core Guide me are specified in [FACILITATOR-MODES.md](FACILITATOR-MODES.md), P-16. The proposed safe-circle layout, text/touch budgets and read-only prototype gates are specified in [ROUND-UI-SPEC.md](ROUND-UI-SPEC.md), P-17. An initial read-only browser preview demonstrates eight paired cards; production features and actual-hardware acceptance remain pending.

## Target
Planned target: Waveshare ESP32-S3-Touch-LCD-1.85; 360x360 round capacitive touchscreen, ESP32-S3, 16MB flash, 8MB PSRAM, QMI8658 accelerometer/gyroscope. This is the original 1.85 model, not 1.85B or 1.85C-BOX.

Verify the exact revision, touch variant, display driver, pins and manufacturer example dependencies on the actual hardware. Do not reuse V1 OLED pins or firmware unchanged.

## Experience
- Compass home screen: Frontier Ops / Trail Guide / Find your next bearing.
- Home prioritizes Ask the Team and Try a Move; fresh opening/reboot shows Home with Ask visually prioritized and no automatic draw. Tapping either mode immediately draws without team/theme setup.
- Tap Another or a deliberate shake on the main card screen spins the needle and reveals a question/activity within the active mode. Suppress shakes in Guide me and other non-card states.
- One core Guide me entry adds listening-first Ask or activity-first Try coaching without repeating the main card, followed by Notice/Follow up; optional Steps/tool notes live inside it later. Linked Ask/Try faces preserve identity and obey release eligibility. More exposes Asked this / Offered this move and Save without crowding the main question.
- Reuse V1 advice meanings and shuffle behavior, with a readable circular layout.
- Ignore extra draw events during animation; tune shake threshold and cooldown on hardware.
- Keep current advice until another draw. Idle dimming and wake preserve it; wake touch must not accidentally skip it.
- Advice works entirely offline. Preserve the optional local Wi-Fi update capability described in [Wi-Fi updates](WIFI-UPDATES.md).

## Architecture
Extract shared advice/deck logic from V1 and keep independent hardware targets. Separate content, pool selection, team/use-event model, persistent storage and hardware UI so they can be tested independently. States include Home, Theme selection, Team selection, Drawing, Advice, Details, Record confirmation, Saved, Team Trail and Idle/Dimmed. Inputs are context-specific: draw gestures cannot confirm recording. Use nonblocking animation. Durability, deduplication, event ordering and migration requirements are specified in PRODUCT-SPEC.md.

## Power and case
USB first. Battery and case are deferred. Before selecting a cell, verify exact connector, polarity, charge current, chemistry and fit for this board. Secure wiring and battery, retain USB access, and measure actual runtime/charging with final firmware. Battery readiness is not established by a capacity estimate.

## Acceptance
- Manufacturer display, touch and IMU examples run on received hardware.
- All content/control/focus bounds fit the measured safe circle; verify longest copy, overflow handling, real font metrics and proposed P-17 touch targets in workshop lighting. Geometric containment is not proof of readability or accurate touch.
- At least twenty deliberate shakes register; ordinary handling does not repeatedly draw.
- Tap fallback is reliable; animation cannot queue unwanted draws.
- Mode/filter pools obey eligibility and uniqueness/boundary-repeat rules, including empty and single-card pools; linked-face navigation does not consume a draw.
- P-16 Guide me/navigation, all-path release eligibility, mode-aware saves/events and legacy migration pass FACILITATOR-MODES.md acceptance. P-17 read-only prototype tasks and actual-hardware UI/touch checks pass ROUND-UI-SPEC.md acceptance.
- Quick start needs no team; recording requires explicit attribution and confirmation.
- Team Trail, saves, feedback and Undo pass product-spec isolation/deduplication tests.
- Trail data survives reboot; interrupted writes, full storage and schema migration have tested explicit outcomes.
- All product-core acceptance checks in PRODUCT-SPEC.md pass; no team data leaks into shared diagnostics.
- Idle/wake preserves advice and does not record or draw accidentally.
- One-hour USB soak and reboot pass.
- Battery operation is only declared ready after runtime and charging tests.

## Deferred
Real north requires a magnetometer; QMI8658 alone cannot supply magnetic heading. Audio, live AI, cloud sync, free-text notes and analytics are deferred. Themes and durable Team Trail history are now core-release requirements, superseding their earlier deferral. Follow-through, favorites and situation shortcuts follow only after the core works. Optional authenticated local firmware updates remain required; advice and team history never require internet access.

Status: production V3 firmware and board emulation are not implemented. An initial read-only browser UI preview exists for facilitator review; it does not establish display/touch/IMU behavior on the actual board.

[Manufacturer reference](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85)
