# Two-mode facilitation — implementation handoff

Status: approved product direction; documentation only. Content examples and UI details below are proposed implementation defaults requiring editorial and prototype review. No new firmware, content schema, tracking migration or hardware behavior is implemented by this document.

This is the detailed companion to [PRODUCT-SPEC.md](PRODUCT-SPEC.md), requirement P-16. The product spec remains canonical. [PLAN.md](PLAN.md) defines sequencing. Preserve the verified V1 fallback and authenticated OTA behavior.

## 1. What changes and why

TrailGuide must coach the facilitator as well as offer activities for teams. A facilitator often needs a useful question and help listening before knowing whether advice is appropriate.

Two modes share the existing eight themes and stable card identities:

| UI label | Stable mode value | Purpose |
| --- | --- | --- |
| Ask the Team | ask | Offer an open question to understand the team's thinking |
| Try a Move | try | Offer one practical activity or experiment |

Ask the Team is the default for a fresh session. Guide me is a detail view available from either mode, not a third mode. No team setup is required to start. Asking another question, waiting, or leaving the team to work can be valid outcomes; the device must not force a transition into advice.

These modes do not create sixteen themes or duplicate the forty-card catalog. A card represents a facilitation intent; the mode changes which face of that intent is presented. The Agent Workflow collection remains an optional, independent content dimension.

## 2. Core interaction

Home presents Ask the Team and Try a Move as its primary large actions, with Team Trail available as a secondary action and the existing team indicator. This replaces the earlier three equal actions Quick bearing / Help my team / Team Trail.

Tapping either mode immediately draws from all eligible cards in that mode; do not require theme selection first. Theme filtering is optional and reachable from the card/navigation controls. Retain Quick bearing as the behavior of an unfiltered draw, not an additional mandatory home step.

Ask card primary content: natural spoken question, title, theme and clear Ask label.
Try card primary content: concise activity/action, title, theme and clear Try label.

Keep Guide me and Another question / Another move accessible. Used it and Save remain deliberate actions, but may live on a secondary action sheet so the circular screen is readable. Navigation affordances must be discoverable; do not depend on undocumented gestures.

Guide me opens:
1. Say — the question or invitation the facilitator can use.
2. Notice — observable things to listen/look for, with uncertainty acknowledged.
3. Follow up — one or two optional open questions, chosen by the human.
4. Optional Try a Move — the related activity, offered without starting or recording it.

Short pages or scrolling are acceptable. Do not cram every field onto one screen. The initial question remains the most prominent text. Back returns to the same card, mode, filter and scroll position where practical.

## 3. A complete example

Theme: Verify; existing card tg-003 CHECK BEARINGS.

Ask the Team:
“Which part of that answer would matter most if it were wrong?”

Guide me / Notice:
Listen for a specific claim and its consequence. If the team cannot name one, help them select a concrete sentence. A fluent answer or confident agreement is not evidence that a claim is correct.

Follow up:
“How could you check that independently?”
“What would you do if you couldn't check it today?”

Try a Move:
Choose one consequential claim and check it against an original source, direct observation or trusted reference. Mark it unverified if the check is unavailable.

The device does not evaluate the team's spoken answer, listen through a microphone, choose a branch automatically or claim that the activity happened.

## 4. Facilitation behavior and editorial rules

- Ask one question at a time; prefer short, open wording usable aloud.
- Avoid questions that hide a prescribed answer, blame a learner or assume AI must be used.
- Notice describes observations, not diagnoses of people or proof of understanding.
- Never infer confidence, ability, consensus, motivation or learning from silence or speed.
- Follow-ups are suggestions, not a scoring rubric or branching assessment engine.
- Invite passing, solo reflection and non-typing participation where appropriate.
- Use the smallest useful intervention: question, optional follow-up, optional activity.
- Help the facilitator pause and listen. Do not auto-advance while a conversation is happening.
- Ask for reasoning or evidence the team can articulate; do not request hidden model reasoning.
- Do not collect answers, audio, transcripts or participant identities.
- Keep general guidance tool-neutral. Existing agent variants retain their separate scope and human checks.
- Guide me is part of core two-mode support; additional AI-prompt templates and tool-specific panels remain optional depth.

## 5. Proposed question coverage across themes

These examples are draft copy, not new card IDs or approved production wording. Map each to an existing intent during content review.

| Theme | Ask the Team | Notice | Optional follow-up |
| --- | --- | --- | --- |
| Explore | What possibilities have you considered so far? | Are alternatives meaningfully different? | What assumption could you change? |
| Clarify | What are you trying to make easier, and for whom? | A concrete outcome versus an output format alone | What would a useful result let them do? |
| Experiment | What is the smallest thing you could try to learn more? | One uncertainty and an observable result | What do you expect to see? |
| Verify | Which part would matter most if it were wrong? | A specific claim and consequence | How could you check it independently? |
| Reflect | What changed in your approach after that attempt? | A concrete change, including a failed approach | What would you try next time? |
| Unstick | Where did progress stop? | Distinguish task ambiguity, tool friction and disagreement | What could you set aside for one small test? |
| Include | How could we make room for another way of contributing? | Available roles and voluntary participation | Would thinking alone or in pairs first help? |
| Decide | What matters most in choosing your next step? | Shared criteria and unresolved trade-offs | What evidence would change your choice? |

## 6. Mode and navigation rules

- Fresh session/reboot defaults to Ask; no persistence of mode is required initially. Durable team data is unaffected.
- Show mode and active theme/filter on card screens. Do not communicate mode by color alone.
- Another question/move draws in the current mode and pool.
- Related Try a Move from an Ask card opens the same card ID/revision's Try face. Related Ask from Try returns to that intent's question.
- Switching the face of the same card does not consume another random draw or reset a deck cycle. A later Another draw uses the destination mode pool's state. Independent pool cycles can legitimately revisit the linked card; only their own draws consume their state.
- A Home mode action is a new draw; a related-face action preserves card identity.
- Theme changes affect only selection; team changes affect attribution, not content or mode.
- Shake, when enabled, draws only on the main Ask/Try card screen. Disable it on Home, Guide me, menus, history, saved lists, confirmations and dimmed screens. Ignore shakes during animation; never queue them.
- Wake consumes the first touch and preserves the card without drawing, saving or recording.
- No timers, automatic follow-ups, forced advice, mandatory team setup or live model calls.
- Ask-only listening is not an error or incomplete workflow.

## 7. Content contract proposal

Inspect content/cards.json and tests before implementing. Keep IDs tg-001 through tg-040, eight themes, original V1 actions and existing agent variants. Do not blindly copy the illustrative structure below into the current schema.

Recommended additive field on a reviewed card:

```json
{
  "facilitator": {
    "ask": "Which part of that answer would matter most if it were wrong?",
    "notice": [
      "Listen for a specific claim and why its accuracy matters."
    ],
    "follow_ups": [
      "How could you check that independently?",
      "What would you do if you could not check it today?"
    ],
    "try_invitation": "Would it help to check one claim together?",
    "review_state": "draft"
  }
}
```

Use the existing action/activity fields for Try, rather than duplicating their text in a competing source. Existing team_question may seed Ask during authoring, but a runtime missing-field fallback must not silently present an instruction as a question. Review Ask wording independently.

Rules:
- Explicitly validate supported review_state values and required nonempty fields.
- Publishing eligibility is mode-specific: unreviewed or missing Ask/Guide me content is excluded from Ask.
- Empty eligible pools show an honest explanation and optional switch/clear filter. One-card pools may repeat without looping.
- Drafts may be visible in an explicitly labeled review prototype, never silently represented as reviewed release content.
- Increment each changed card revision and regenerate readable catalog/derived guides in the same change.
- Optional additive fields may retain the existing content schema version only if every current consumer/test tolerates them. Bump the schema if required meaning or compatibility changes.
- Store Ask/Guide me details in the recorded revision or snapshot so later edits cannot rewrite history.
- Do not change the flashed V1 deck just to make it match the expanded catalog.

First author/review representative pairs across all eight themes, then complete Ask/Guide me coverage for the forty-card catalog. This is editorial enrichment, not a requirement for forty new cards.

## 8. Pools, saves and history

Pool keys include mode, theme/situation filters and optional collection. Retain P-11 no-repeat rules within each eligible pool. Mode switching must not reset unrelated pool states. Related-face navigation is not a draw.

A save refers to the underlying card plus originating mode, so reopening it restores the intended face. Uniqueness is per existing scope + card ID: saving its other face does not create a duplicate save. If already saved, show Saved; do not silently change its originating mode. If face-specific saves are later desired, that requires a separate requirement change.

Used it stays an explicit confirmed action. Record the mode actually used: ask or try. Merely drawing, reading Guide me, switching faces or opening a saved card is not use.

- Ask means the facilitator actually used the question with the team.
- Try means the facilitator actually offered/started that activity, not that the team completed it successfully.
- Opening the linked activity after recording a question does not automatically record a Try.
- The same card can have distinct deliberate Ask and Try use events. Deduplication includes the pending action/mode identity so rapid taps cannot duplicate an event, while explicit Use again remains allowed.
- Every confirmation names the destination team and mode.
- Team Trail labels Asked / Tried. These describe the recorded intervention, not measured learner performance.
- Themes explored count non-reversed use events as before; do not equate counts with mastery.
- Undo and optional feedback attach to the exact event. Helpful remains subjective.
- Old records missing mode display Legacy/unspecified. Never guess that an old use was Asked or Tried.
- Preserve saves and events during migration; reject unsupported newer storage schemas without overwriting them.

Mode-aware tracking is required before releasing two-mode Used it/history, not a prerequisite to a read-only prototype. If tracking is unavailable, disclose it and do not fake persistence.

## 9. V1 / V2 delivery boundaries

V1 remains the verified twelve-card SH1106 fallback. This documentation does not authorize replacing its one-button controls or reflashing it. Optional OLED mode experiments require a separately tested input design; do not casually overload BOOT with undiscoverable long-press actions.

V2 is the main target for the two-mode UI. Inspect actual board/revision and retain manufacturer bring-up gates. Keep shared content/deck logic independent of display, input and storage.

Suggested state additions: ModeCard(ask|try), GuideMe, RelatedFace, ActionSheet. Extend existing state machine rather than creating duplicate implementations for each mode. Core Ask/Guide me content should be offline; memory/flash budget must be measured on the actual board.

## 10. Acceptance checklist

Content:
- All existing IDs, V1 action wording, theme membership and agent variants are preserved unless an explicit reviewed revision changes them.
- Every release-eligible Ask has a real question, Notice and at least one follow-up.
- Follow-ups are optional, readable and nonjudgmental; no fake response interpretation.
- Canonical JSON and generated catalog agree.
- Editorial review is distinct from structural test passing.

Selection/navigation:
- Fresh session is Ask; either mode starts without a team or theme selection.
- Mode/filter labels are visible, and every theme is reachable in either mode.
- Draw pools pass no-repeat, boundary, zero/one-card and eligibility tests.
- Linked-face navigation preserves card/revision and does not consume a draw.
- Back from Guide me preserves card/context.
- Scrolling, wake, shaking in disabled states and rapid input cannot record use or queue draws.
- Remaining text and controls are readable inside the actual 360x360 circle; use multiple pages where needed.

Tracking/storage:
- A read-only prototype records nothing.
- Confirmed Ask/Try events retain correct team, mode, revision and ordering.
- Team switch, cancellation, double taps, explicit reuse, Undo and feedback target the right event.
- Same-card Ask and Try can both be deliberately recorded.
- Saves remain deduplicated by scope/card and reopen the stored mode.
- Legacy records do not acquire fabricated mode labels.
- Reboot, full storage, corruption, interrupted writes/migrations and unknown-schema paths pass existing durability gates.

Regression:
- Existing V1 firmware/OTA tests still pass if shared code changes.
- Docs-only changes do not require resetting or reflashing hardware.
- No new hardware readiness is claimed from host tests or a browser prototype.

## 11. Next steps for the development host

1. Pull current main without overwriting local changes. Read AGENTS.md, PRODUCT-SPEC.md, this document, PLAN.md and BRINGUP.md. Inspect current state; do not assume earlier observations are live.
2. Report how existing card schema, generator and tests can support paired faces without duplicate IDs.
3. Author representative Ask/Notice/Follow-up pairs spanning the eight themes; keep review state explicit.
4. Build a 360x360 interactive prototype for default Ask, Try, Guide me, linked-face navigation and action sheet. Use synthetic teams/history only.
5. Review the interaction with the facilitator. Refine labels/layout without losing stable mode values.
6. Extend remaining catalog entries and content tests; regenerate derived docs.
7. Implement/test mode-aware pools and events/storage migration before enabling tracking.
8. Integrate into V2 after actual-hardware bring-up; preserve V1 fallback and OTA compatibility.
9. Run relevant existing suites plus new mode tests. Record executed checks, observed results and remaining acceptance in BRINGUP.md.
10. Publish only intended sources/docs/tests. No credentials, private operational context or real team responses.

Do not read or modify an active development host session remotely as part of this handoff. The local development agent continues execution from the repository.
