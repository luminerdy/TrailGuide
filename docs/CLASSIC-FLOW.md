# Classic — prompt-deck flow

**Classic offers one curated question or practical move at a time.** Use it for a quick spark, then return attention to the person or team.

This maps the installed V3 `v3-0.1.20-shake-calibration` experience, not the broader planned Team Trail interface. Read alongside the [Coaching flow](COACHING-FLOW.md) and [project specification](PROJECT-SPEC.md).

**Approved direction: Coaching-first, with revised V3 wording as its content foundation.** This is not a startup-default change or a content merge: Classic still boots by default and remains an intact optional quick-prompt route. Coaching currently uses its own authored layer plus canonical references; neither all twelve Classic pairs nor the whole forty-card catalog has been merged into Coaching.

```mermaid
flowchart TD
    Boot([Start or reboot: Ask is the default mode]) --> Compass["Decorative compass<br/>No automatic card draw"]
    Compass -->|Tap compass| Options[Classic Options]
    Compass -->|Complete turn over and back, or sustained shake| Ask
    Options -->|Ask the Team| Ask[Set mode to Ask]
    Options -->|Try a Move| Try[Set mode to Try]
    Ask --> Draw[Draw one card in the active Ask or Try mode]
    Try --> Draw
    Options -->|Instant text toggle| Options
    Options -->|Sound OFF or ON: shared RAM preference| Options
    Options -->|Back| Compass
    Options -->|Coaching| Coaching([Enter Coaching compass: separate flow])
    Draw -->|Animated presentation| Reveal["Whole answer-bearing face emerges<br/>6-second reveal"]
    Draw -->|Instant text or safe rendering fallback| Hold["Selected card alone<br/>Additional 5-second reading hold"]
    Reveal -->|Completes, or tap to finish this same reveal| Hold
    Hold -.-> Cue["New draw now readable: optional settlement chime<br/>Sound ON only; busy cues dropped"]
    Hold -->|Hold finishes, or tap to show controls| Card["Same question or activity<br/>Controls available"]
    Hold -->|New complete turn over and back, or rearmed sustained shake| Draw
    Card -->|Another, complete turnover, or sustained shake| Draw
    Card -->|Guide me| Guide[Listening cues or activity guidance]
    Guide -->|Back: no draw| Card
    Card -->|More text only if text remains below; scroll: same card| Card
    Guide -->|Conditional More text or scroll: same guidance| Guide
    Card -->|Home| Compass
    Guide -->|Home| Compass
```

## What the choices mean

- **Ask the Team:** a spoken, open question from the Classic selection.
- **Try a Move:** a practical activity from that same selection of card identities.
- **Guide me:** support for the selected card, not a new draw. Ask guidance emphasizes listening; Try guidance emphasizes running the activity.
- **Another:** a fresh shuffled draw in the active mode. Each mode has its own bag; the twelve selected identities do not repeat within its cycle, and reshuffling avoids an immediate boundary repeat.
- **Home:** returns to the compass without drawing. A turnover or sustained shake from the compass explicitly draws Ask, even after a Try card; either gesture from a settled card retains that card's active mode. Choose Try a Move in Options when an activity is wanted.

## Gesture, reading and navigation rules

- Touch mode selection and **Another** provide alternatives to motion. No essential prompt requires a gesture.
- A qualified inversion blacks the existing compass/card; a completed return selects exactly one new card. A canceled or invalid cycle restores the previous scene without drawing.
- Motion draws are eligible on the compass or a settled main card, not in Options, Guide me, an active reveal, a held touch or scrolling.
- The five-second settled reading hold is separate from reveal-busy: a new eligible completed turn can draw during that hold.
- A tap can finish the same reveal and start its hold; a subsequent settled tap can expose controls. Neither consumes another draw.
- Guide/Back, scrolling and ordinary rerendering preserve the selected identity and do not replay the reveal. Full text remains accessible.
- Instant text bypasses the animation, not the card's identity or complete-copy access. A safe allocation/capture fallback is not a successful animation.

Classic no longer shows the constant ASK/DRAFT/flip footer; canonical review states remain draft. Classic **More text** and the Coaching scroll hint appear only when actual laid-out text remains below the visible viewport, including the opener’s 14px bottom clipping mask—not merely because padding permits scrolling. Mode centers, Options targets, six path controls, content, typography and reveal/hold timing are otherwise retained.

A deliberate sustained shake of roughly one second is an additional draw route, alongside turnover and touch; it goes straight to the existing reveal without requiring turnover. It is eligible only on Classic Home or a main card (Home draws Ask; a card retains active Ask/Try), or Coaching Opening (compass or opener card). Busy reveal, pointer contact, scrolling, Options, guidance, paths/questions/experiments and closings suppress motion. One sustained burst draws once; continuing to rattle cannot queue another card. Recognition requires 400ms of fresh quiet before intent, 900ms of credited fresh motion and ten opposing peaks, then a 2200ms cooldown followed by fresh quiet to rearm. These are implementation safeguards, not a prescribed handling ritual.

## Optional settlement sound

**Sound** is one shared, RAM-only preference in Classic Options and Coaching Options.
It defaults **OFF after every reboot**. Booting and toggling either way are silent.
When ON, an accepted new Classic card or explicitly requested new Coaching opener
can play one **240ms, 500Hz decaying settlement chime**, using a nominal PCM peak
of **1536**. It begins when that draw becomes readable: normally after the six-second
reveal, or immediately on same-card reveal completion, Instant text or safe fallback.
The additional five-second reading hold is unchanged. Initial Coaching entry is
silent even though it selects its first opener; context restoration, Guide/Back,
path browsing, scrolling and ordinary navigation never cue. Busy cues are dropped,
not queued. OFF requests cancellation of active playback; switching ON again does
not revive that canceled cue. Audio uses a low-priority TX-only worker; the ready
main loop services it once after UI/IMU work. No RX, DIN/MCLK, microphone, storage,
networking or OTA is enabled by this feature.

Button fills are `#0c1721` normal / `#1a2b38` pressed; apart from footer/conditional-cue cleanup, accepted fonts, icons, spacing and touch areas are retained. Physical Sound menu/mute/chime acceptance and narrower-face readability remain pending. The earlier single very soft bench tone is not acceptance of this cue.

The UI cleanup is physically accepted on v3-0.1.19 and retained on v3-0.1.20. Natural Coaching-compass shake success has qualitative positive consistency feedback; no separate Classic shake trial or numerical reliability count is inferred. The original intermittent turnover-stuck-compass/blank issue remains OPEN, not fixed by adding shake. See [current evidence and acceptance limits](V3-TRIAL.md).

## Boundaries

Classic is offline, authored draft content—not live AI, a diagnosis or a record of use. Draws, reading and navigation do not record learning, team activity or outcomes. There is no implemented Team Trail, saved-card or history interface on V3.

Implementation references: [Classic UI](../trailguide_v3/ui.h), [deck model](../trailguide_v3/model.h), [turnover recognition](../trailguide_v3/gesture.h), sustained-shake detector `shake.h`, [whole-face renderer](../trailguide_v3/window_reveal.h), [current device evidence](V3-TRIAL.md).
