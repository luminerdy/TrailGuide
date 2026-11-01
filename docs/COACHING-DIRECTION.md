# Coaching direction — help the facilitator and the learners

Status: endorsed product direction; research synthesis, training materials, new interfaces and effectiveness validation are pending. This document records design intent, not a claim that these approaches have been validated for every learner or that the proposed features are implemented. See [P-19 in the product specification](PRODUCT-SPEC.md#p-19-facilitator-development-and-calibrated-reliance--research-gated-direction).

## Purpose

TrailGuide should help a facilitator invite participation, listen, encourage useful thinking and offer an appropriate next step. It should support both the team's learning and the facilitator's development—not simply supply more questions or persuade people to use AI.

The goal is appropriate reliance: deciding whether a particular AI contribution is useful for a particular task, what needs checking, and when another approach is preferable. Do not assign people permanent labels such as over-truster, skeptic, weak learner or disengaged participant.

## Working coaching loop

**Notice → Invite → Listen → Offer a small next step → Reflect.**

This is a proposed design scaffold, not an empirically validated TrailGuide protocol.

- **Notice:** attend to observable statements/actions; distinguish observation from interpretation. Silence alone does not authorize a conclusion about understanding, motivation, agreement or willingness to speak.
- **Invite:** ask one accessible, non-leading question. Allow thinking time, an alternative response format and passing.
- **Listen:** make room for the learner's account before adding an explanation. Follow-ups are optional, not an interrogation or a branching assessment.
- **Offer:** ask whether a hint, example, small experiment or another approach would help. A learner may decline an activity or AI use.
- **Reflect:** review the facilitator's own choices and the evidence available, without rating people or inventing an outcome.

## Examples for rehearsal and later editorial review

These are synthetic draft examples, not new card IDs or approved production wording.

### A learner is hesitant to respond

Opening: “Would a hint, an example, or some time to think be most useful?”

Facilitator cue: leave room for the answer; do not make someone explain why they are passing. Offer a paired, written or other accessible alternative where appropriate.

Possible next question: “Which part would be useful to unpack together?”

### A fluent AI answer is accepted quickly

Opening: “What would you check before relying on this?”

Facilitator cue: focus on a consequential claim and the intended use, not on whether someone sounds confident. Distinguish useful assistance from evidence that a claim is correct.

Possible move: choose one relevant claim and compare it with an original source, direct observation or an independent test. If no check is available, retain the uncertainty rather than manufacture reassurance.

### Someone reasonably doubts AI's usefulness

Opening: “Would a small, low-risk comparison help you decide whether this is useful here?”

Facilitator cue: skepticism may be justified. Agree on what a useful result would look like; include a non-AI option and permission not to proceed.

Possible next question: “What would make the result worth using—or not using?”

### Progress appears blocked

Opening: “What have you tried, and what did you notice?”

Facilitator cue: ask about the task before assuming a personal deficit. Offer a manageable next step without taking over the part the learner wants to practice.

## Before, during and after

### Before: facilitator practice, initially outside the device

Choose one micro-skill, rehearse a short synthetic exchange and compare possible responses. Initial candidates are inviting uncertainty, pausing, asking permission before helping, paraphrasing tentatively, and checking an AI claim without shaming its user.

A short companion exercise or written rehearsal is the smallest proposed starting point. A new practice mode, simulated learner or AI role-play system is not authorized implementation merely because this direction is endorsed. If simulated dialogue is explored, label it synthetic and do not treat its feedback as a measure of real coaching competence.

### During: preserve the glanceable live tool

Keep Ask the Team / Try a Move and the single Guide me entry. Main question/action first; guidance supplies a useful cue, an optional follow-up and, where appropriate, a small activity. Avoid repeating the heading's instruction in its body.

V2 has only 172x320 pixels. Improve the eight representative pairs before expanding the deck or adding menus. Situation shortcuts may be considered later under P-09; do not add a third home mode or multiple help buttons without a separate interaction review.

### After: optional facilitator reflection

Possible prompts:

- “Where did I make room for their thinking?”
- “Where did I offer help before asking what would help?”
- “What did they explain, test or choose for themselves?”
- “What might I try differently next time?”

Reflection is optional. It does not imply that a learner improved, agreed or trusted appropriately. Do not require free-text recording or participant information.

## Product boundaries

- Preserve learner choice, accessible alternatives and a non-AI path; do not promise one method will work for everyone.
- No hidden recording, microphone-based interpretation, surveillance, learner profiles, diagnoses, ability/confidence scores or participation leaderboards.
- Planned Team Trail records deliberate facilitator interventions, not inferred learning or psychological states. Viewing a card is not using it.
- Keep content offline and tool-neutral where practical. Tool-specific examples need their own source/version review.
- Do not turn research findings from one school, workplace, discipline or task into universal advice for adult AI workshops.

## Evidence and a small pilot

Research should distinguish established coaching/learning work from emerging AI-specific findings. Assess both benefits and risks, and separate supported task performance from unaided learning/transfer. Identify study design, context, limitations, contradictory findings and corrections. Source-derived claims require citations; search snippets and plausible model summaries are not evidence.

The research synthesis is pending at this checkpoint. Its scheduled private collection does not make it a published result. Review the actual findings before converting the proposed loop or example wording into claimed best practice.

Proposed pilot sequence:

1. Review evidence and co-design wording with facilitators and learners with different access/participation needs.
2. Rehearse the synthetic scenarios; identify leading, repetitive, confusing or pressure-inducing wording.
3. Trial a small selection of existing draft pairs with consent and an option to pass.
4. Observe whether a facilitator can find a suitable prompt, leave space for an answer and choose an appropriate next move. Use voluntary feedback and observable examples, not hidden inference.
5. Revise one small part, retain clear limitations, and repeat before expanding the interface or enabling tracking.

AI usage, speed, prompt count, clicks and polished outputs are not substitute success measures. Define evidence of usefulness with the participants, without collecting identifying responses in the repository.

## Delivery order

1. Research-backed coaching brief and a small facilitator-practice companion.
2. Editorial/interaction review of the existing eight pairs and the revised guide flow.
3. A bounded human pilot and explicit decision about the next content or UI change.
4. Separate implementation/acceptance for additional pairs, situation navigation and durable Team Trail.

This work can proceed as a content/research track without bypassing existing firmware, privacy, storage or release gates. See [PLAN.md](PLAN.md), [FACILITATOR-MODES.md](FACILITATOR-MODES.md) and the [handoff](HANDOFF.md).
