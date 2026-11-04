# Coaching — conversation-guide flow

**Coaching helps the facilitator open, listen, explore and release ownership of the next step.** The guide chooses the pace and direction; the device does not diagnose the team or listen to the conversation.

This maps the installed V3 `v3-0.1.18-coaching-compass` experience. It is a flexible navigation map, not a required sequence. Read alongside the [Classic flow](CLASSIC-FLOW.md) and [project specification](PROJECT-SPEC.md).

```mermaid
flowchart TD
    Entry([Classic compass: tap, then Coaching]) --> Compass["Coaching compass: wordless full gold/blue arrow<br/>Progress / Stuck / Unclear / Exploring / AI Results / Team<br/>Opener retained silently"]
    Compass -->|Select any of six paths| Context["Your selected path<br/>Restore its last question or experiment face"]
    Context -->|Question face| Question[Question for this path]
    Context -->|Saved experiment face| Experiment[Optional small experiment]
    Compass -->|Invisible center: Options| Options[Coaching Options]
    Compass -->|Complete turn over and back| NewOpener["Draw another shuffled opener<br/>Optional reveal and reading hold"]
    Options -->|Another opener| NewOpener
    NewOpener --> Opener["Optional opener card<br/>6-second whole-face reveal + 5-second hold"]
    Opener --> Listen["Listen: human activity, off-device"]
    Opener -->|Select any of six paths| Context
    Opener -->|Options| Options
    Opener -->|Complete turn over and back| NewOpener
    NewOpener -.-> Cue["Only this explicit new draw: optional chime when readable<br/>Not initial entry or restoration; Sound ON only"]
    Options -->|Instant text toggle| Options
    Options -->|Sound OFF or ON: shared RAM preference| Options
    Options -->|Classic| Classic([Return to Classic compass])
    Options -->|Back: preserve entry variant| Variant{Compass or opener card?}
    Variant -->|Compass| Compass
    Variant -->|Card| Opener
    Question -->|Another: authored order| Question
    Experiment -->|Another: authored order| Experiment
    Question -->|Try - optional| Experiment
    Experiment -->|Question| Question
    Question -->|Paths| Picker["Choose a path<br/>Or return to Opening / choose Next step"]
    Experiment -->|Paths| Picker
    Picker -->|Select or switch path| Context
    Picker -->|Opening: same opener retained| Compass
    Picker -->|Back| Previous[Return to previous context and scroll]
    Question -->|Guide me| Guide[Private guidance for this face]
    Experiment -->|Guide me| Guide
    Options -->|Guide me| Guide
    Guide -->|Reminders| Reminders[Private listening reminders]
    Options -->|Reminders| Reminders
    Guide -->|Back| Previous[Return to previous context and scroll]
    Reminders -->|Back| Previous
    Reminders -->|Next: authored order| Reminders
    Guide -->|Opening| Compass
    Reminders -->|Opening| Compass
    Question -->|Release at any point| Closing[Optional next-step questions]
    Experiment -->|Release at any point| Closing
    Options -->|Release without choosing a path| Closing
    Picker -->|Next step| Closing
    Closing -->|Next: authored order| Closing
    Closing -->|Opening: same opener retained| Compass
    Closing -->|Back| Previous
    Closing -.-> Ownership["People choose their next step<br/>Nothing is recorded by the device"]
```

## The six paths

These are selectable conversation contexts, **not diagnoses or permanent classifications**:

- **Progress — 12 o'clock:** explore what works, learning and possibilities to build on.
- **Stuck — 2 o'clock:** understand obstacles without immediately rescuing.
- **Unclear — 4 o'clock:** clarify goals, expectations and uncertainty.
- **Exploring — 6 o'clock:** examine alternatives and useful experiments. The small ring caption is **Explore**.
- **AI Results — 8 o'clock:** understand the gap between expected and actual AI output without assuming the prompt is the problem. The small ring caption is **AI**.
- **Team — 10 o'clock:** invite perspectives, participation and shared understanding.

Each path has four questions and three optional experiments. Questions and experiments cycle in authored order, with separate remembered positions. Choosing a path restores that path's last question/experiment face rather than always forcing its first question.

## Freedom to adapt

- The arrows show available routes, not automatic advancement. Skip, adapt, switch paths or finish when appropriate.
- **Release** has no question quota and requires no experiment. It is reachable from Options without choosing a path, from question/experiment screens, through **Next step** in the picker, and from private Guide/Reminders screens.
- **Guide me** and **Reminders** are for the facilitator, not questions to read to the team. Their **Back** route restores the actual prior context; the shared return box does not force a particular path.
- The three closing prompts support the people's own next step. The device does not capture, confirm, save or assess their response.
- **Opening** returns to the compass, preserving the exact prior opener and its saved scroll context without consuming a draw. Selecting **Classic**, then re-entering Coaching, also retains the opener within the current boot. Reboot clears this RAM-only context.

## Opening and presentation rules

- Six opening questions use their own shuffled pool. Only a deliberate new opener consumes that pool.
- Entry shows the compass, not an opener question: the model selects/retains the opener silently. Another opener from Options or a qualified completed turnover switches to the legacy blue opener-card reveal, unless Instant text or a safe fallback bypasses it.
- The compass has six labelled path icons and an invisible **136x64 center Options** target. Its independent decorative LCG wandering never selects a path, changes the opener or consumes a draw. Options Back restores whichever Opening variant opened it.
- Motion draws operate **only on the Opening screen**. Flipping while browsing a path, help, closing or Options cannot replace that content.
- Qualified inversion blacks the old opening scene; completed return starts the six-second reveal, followed by five additional seconds for reading. Cancellation restores the previous scene. Tap alternatives and early reveal/hold completion remain available.
- The explicit opener-card variant (not compass entry) retains a blue face of 192x112 inside the unchanged 256x140 capture panel, with 18px body text, 168px opening wrap and 12px ring/Options captions. Invisible touch targets remain generous; long text is scrollable.

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

Button fills are `#0c1721` normal / `#1a2b38` pressed; main-card pixels, icons, fonts, spacing and existing touch areas are unchanged. The quieter button appearance is physically approved and preserved. Physical Sound menu/mute/chime acceptance and narrower-face readability remain pending. The earlier single very soft bench tone is not acceptance of this cue.

The new Coaching Home preview is approved, but on-device Home/touch acceptance is pending. The original physical flip/blank issue remains unresolved; earlier touch recovery is not a fix claim.

## Boundaries

Coaching is offline, authored draft guidance. There is no microphone, live AI, automatic team assessment, mandatory sequence, participant tracking or persistent conversation history. The learner-facing loop remains **Define → Direct → Check → Adjust**; these paths are optional private facilitator aids, not a replacement curriculum.

Implementation references: [coaching UI](../trailguide_v3/coaching_ui.h), [context/navigation model](../trailguide_v3/coaching_model.h), [authored coaching content](../content/coaching.json), [browser/native distinction](COMPASS-COACHING-PREVIEW.md), [current device evidence](V3-TRIAL.md).
