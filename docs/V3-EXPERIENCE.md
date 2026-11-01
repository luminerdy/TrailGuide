# V3 experience study — shake, discover, reveal

Status: endorsed interaction direction; separate browser experience study, not V3 firmware or physical shake verification. Canonical requirements: [P-20](PRODUCT-SPEC.md#p-20-v3-playful-idea-draw-and-gradual-reveal), [V3 hardware contract](V3-SPEC.md) and [shared Ask/Try contract](FACILITATOR-MODES.md). The existing [full-flow round preview](../prototype/index.html) remains separate and unchanged.

## Intent

A deliberate shake offers a possible next idea for a team. After a short settling beat, the whole answer emerges together from darkness, like a Magic 8 Ball: complete text fades into view, without typing individual letters or sequential words. Keep layout/wrapping fixed. The shared moment should feel playful, while the result remains a readable question or practical move that people can accept, adapt or ignore.

This is optional novelty, not the core mentoring programme, a compulsory participation ritual or evidence of better learning. The project gives the activity context; randomness does not make every card appropriate. The device reveals curated content, not a live AI answer or pretend AI thinking.

## Proposed device behavior

1. Fresh opening shows Home without drawing. Choose **Ask the Team** or **Try a Move**; no team setup is required.
2. On an eligible main-card screen, **Another** or one deliberate shake uses the same draw pipeline, active mode/filter and release eligibility. Ask reveals a question; Try reveals a concise action.
3. Select one card, settle briefly, then reveal its text progressively. Preserve its identity, revision and complete original copy throughout; no runtime paraphrase or second draw during the reveal.
4. **Reveal all** completes that same card immediately. An **Instant text** preference skips both settling and gradual appearance. Respect reduced-motion preferences where available. Essential actions never require shaking or waiting for an animation.
5. Keep the complete copy readable and scrollable/pageable. Restore normal **Guide me / Another** navigation when ready; revealing the main text does not enter guidance, mark use, save, rate or attribute an event.

Use a nonblocking state machine: `ready -> settling -> revealing -> ready`. Accept one draw per deliberate input. Ignore—not queue—additional shakes and Another taps while settling/revealing. A short hardware cooldown and a return-to-rest condition prevent a sustained shake from causing repeated draws; actual thresholds, timing and orientation tolerance need measured IMU traces and human review.

Suppress motion draws on Home, in Guide me, menus, team/theme selection, history, confirmations, dim/wake transitions and updating/error overlays. Navigation or mode/filter changes cancel the old reveal and invalidate callbacks; no late text overwrite or delayed draw. A changed selection must follow the normal destination-context rules, never a stale callback. Waking preserves the current card and consumes its first input.

Pools shuffle through eligible IDs before reshuffling and avoid immediate boundary repeats when more than one item exists. Empty pools explain the absence; a one-card pool may repeat without hanging. Randomness is bounded by mode/filter, not personalized by inferred learner traits.

A decorative compass beat may accompany the settling phase if it remains restrained and optional. It is not a magnetic heading instrument. Do not add sound, microphone input, cloud requests or motion-triggered recording.

## Browser study: what to try

The existing [v3-experience.html](../prototype/v3-experience.html) is a self-contained offline **legacy timing/input study**. It still uses character-by-character reveal and has not been updated to the corrected whole-answer emergence direction. Do not treat its typing appearance as the intended finished V3 effect. It remains useful for reviewing simulated input, pool selection, skip and cancellation:

- Start with no automatic draw, choose Ask or Try, then use **Simulate shake** or **Another**.
- Watch the progressive reveal; try **Reveal all** and confirm the same idea remains.
- Try **Instant text**, both modes, repeated inputs and keyboard controls.
- Consider whether the delay is enjoyable or irritating and whether the text is easy to read. Passing, using touch or disabling the effect should feel ordinary.

The study uses the same eight canonical **draft** pairs as the existing round preview, with shuffled draws instead of V2's fixed hardware-review order. It tests the draw/reveal experience only: no real sensor integration, theme picker, Guide me flow, team roster, use events, storage or production eligibility engine. The circle includes simplified Ask/Try controls; outside-circle simulation/preferences controls are review tools, not a proposed complete device layout. It does not replace P-17's full navigation review.

The proposed reveal is deliberately short and capped; its exact pacing remains a review input, not a measured hardware guarantee. Browser timers, logical pixels and synthetic shake clicks do not establish display frame rate, touch accuracy, gesture sensitivity, RAM use, battery life or learning benefit.

## Regenerate and test

From the repository root:

```sh
python3 tools/build_v3_experience.py
python3 tools/build_v3_experience.py --check
python3 -m unittest discover -s tests -p 'v3_experience_test.py' -v
```

Edit the template, not the generated artifact. Canonical card wording, IDs, revisions and draft review states remain unchanged. The standalone artifact makes no network requests and stores no data.

## Before a V3 hardware claim

- Identify the exact board/revision, verify the manufacturer display/touch/IMU examples and confirm the actual sensor, bus and orientation. A manufacturer-listed QMI8658 is not a physical-board test.
- Test deliberate shake versus ordinary handling, carrying, setting down, rotation, touch/scroll and waking. Tune from measured traces; verify at least the V3-SPEC deliberate-shake acceptance sample without treating it as a statistical reliability claim.
- Prove one draw per gesture with rest/cooldown, no queued draws, all suppressed states and cancellation/skip behavior. Repeat with slow frames and interrupted navigation.
- Check readable complete copy, real touch targets, responsive reveal/skip, instant text and offline behavior on the round display. Verify authenticated updates remain safe and usable.
- Obtain human feedback on fun, usefulness, pressure and distraction. Novelty can be removed without losing the mentoring materials or core touch workflow.

Record browser evidence separately from physical evidence in [BRINGUP.md](BRINGUP.md). Preserve the current V2 trial and V1 fallback; this experiment does not authorize a flash or firmware port.
