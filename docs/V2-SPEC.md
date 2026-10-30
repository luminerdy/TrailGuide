# V2 — Round Trail Guide Compass

## Product requirements

The [living product specification](PRODUCT-SPEC.md) is the canonical source for the touchscreen experience: Quick bearing, eight themes, team selection, deliberate Used it tracking, saved ideas and durable Team Trail history. Follow-through, favorites, situation shortcuts and expanded guidance are phased enhancements. See [PLAN.md](PLAN.md) for delivery gates. None of these features is implemented yet.

## Target
Ordered Waveshare ESP32-S3-Touch-LCD-1.85, expected November 1, 2026: 360x360 round capacitive touchscreen, ESP32-S3, 16MB flash, 8MB PSRAM, QMI8658 accelerometer/gyroscope. This is the original 1.85 model, not 1.85B or 1.85C-BOX.

Verify the exact revision, touch variant, display driver, pins and manufacturer example dependencies on arrival. Do not reuse V1 OLED pins or firmware unchanged.

## Experience
- Compass home screen: Frontier Ops / Trail Guide / Find your next bearing.
- Tap or deliberate shake spins the needle and reveals advice.
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
- All cards fit the circle and are readable under workshop lighting.
- At least twenty deliberate shakes register; ordinary handling does not repeatedly draw.
- Tap fallback is reliable; animation cannot queue unwanted draws.
- Theme pools obey uniqueness/boundary-repeat rules, including empty and single-card pools.
- Quick start needs no team; recording requires explicit attribution and confirmation.
- Team Trail, saves, feedback and Undo pass product-spec isolation/deduplication tests.
- Trail data survives reboot; interrupted writes, full storage and schema migration have tested explicit outcomes.
- All product-core acceptance checks in PRODUCT-SPEC.md pass; no team data leaks into shared diagnostics.
- Idle/wake preserves advice and does not record or draw accidentally.
- One-hour USB soak and reboot pass.
- Battery operation is only declared ready after runtime and charging tests.

## Deferred
Real north requires a magnetometer; QMI8658 alone cannot supply magnetic heading. Audio, live AI, cloud sync, free-text notes and analytics are deferred. Themes and durable Team Trail history are now workshop-core requirements, superseding their earlier deferral. Follow-through, favorites and situation shortcuts follow only after the core works. Optional authenticated local firmware updates remain required; advice and team history never require internet access.

Status: specification only; no V2 firmware or simulator implemented.

[Manufacturer reference](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-1.85)
