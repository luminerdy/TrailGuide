# Card catalog

Forty draft cards across eight themes, five per theme. The original twelve V1 cards are preserved; twenty-eight cards are new. This is editorial content for review, not a firmware release or a claim of touchscreen fit.

Canonical structured source: [content/cards.json](../content/cards.json). Requirements: [PRODUCT-SPEC.md](PRODUCT-SPEC.md).

## Agent Workflow collection

Ten existing cards now include optional CLI-agent guidance for Codex and Claude Code. The total stays forty, with eight themes. See the [Agent Workflow guide](AGENT-WORKFLOW.md) for the suggested sequence, safety boundaries and tool-instruction references. Collection UI and tool-note display are planned, not implemented.

## Using the catalog

- Each card has a short action plus optional deeper guidance. Time boxes are suggestions, not timers.
- AI prompts are optional templates for a separate approved AI tool. TrailGuide works offline and does not send prompts or team data anywhere.
- Replace placeholders only with synthetic or approved non-sensitive material. No participant details, confidential notes or credentials.
- Adapt activities for access needs and group size. Invite participation without forcing disclosure or treating silence as agreement.
- All entries remain drafts pending facilitator review and actual-screen layout checks. New cards have no OLED layout claim.
- IDs remain stable even if titles change. Increment a card revision when its content changes; retain prior revisions or snapshots when history is implemented.

## Themes

- **Explore**: We need possibilities.
- **Clarify**: We're not sure what we're solving.
- **Experiment**: We need something small to try.
- **Verify**: Can we trust this?
- **Reflect**: What did we learn?
- **Unstick**: We're going in circles.
- **Include**: We need more voices involved.
- **Decide**: We need to choose and move.

## Explore

We need possibilities.

### tg-001 - SCOUT

Revision 1 | Draft | Preserved V1 card | About 3 minutes

**Bearing:** Ask AI for three very different approaches.

**Ask the team:** What would a genuinely different approach look like?

**Try it:** Name the current approach. Ask for three alternatives with different starting assumptions, then choose one difference worth exploring. Do not select a winner yet.

**Optional AI prompt:** For this non-sensitive challenge: [challenge], suggest three approaches with different starting assumptions. State each assumption and one trade-off. Avoid three minor variations of the same idea.

### tg-005 - NEW PERSPECTIVE

Revision 1 | Draft | Preserved V1 card | About 4 minutes

**Bearing:** Ask AI to critique the idea as its intended user.

**Ask the team:** What might the person using this find difficult?

**Try it:** Describe an intended user without identifying them. Ask AI for possible difficulties, then separate plausible concerns from guesses. Choose one question to ask a real user; simulated feedback is not user research.

**Optional AI prompt:** Review this idea: [idea], for this intended user: [generic needs]. Suggest possible frustrations and label them as hypotheses, not real user feedback. Give one question we could ask an actual user.

### tg-013 - SIDE TRAIL

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Borrow an approach from a different field. Name where the analogy breaks.

**Ask the team:** Who solves a similar problem in a very different setting?

**Try it:** Pick a different field, such as libraries or gardening. Borrow one useful pattern for the challenge. Name one important difference that makes the analogy unreliable, then keep only the useful part.

**Optional AI prompt:** For [challenge], suggest an analogy from another field. Explain one transferable pattern and one important limit of the analogy. Do not treat the analogy as evidence.

### tg-014 - HUMAN FIRST

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Sketch how to solve the problem without AI. Then name one place AI might help.

**Ask the team:** What would we do if AI were unavailable?

**Try it:** Draw a three-step human-only workflow. Circle one step where AI might reduce effort, and keep one step where human judgment must remain. Check whether AI adds value at all.

**Optional AI prompt:** Here is a human-only workflow: [steps]. Suggest one limited place AI might help and one reason not to use it. Keep the accountable human decision visible.

### tg-015 - WIDER HORIZON

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Choose one constraint to relax and one to keep. Explore what becomes possible.

**Ask the team:** Which constraint is real, and which have we simply assumed?

**Try it:** List three constraints. Mark which are confirmed and which are assumptions. Temporarily relax one assumption, not a safety or privacy requirement, and sketch a new option.

**Optional AI prompt:** For [challenge], we must keep [non-negotiable constraint]. If we relaxed [assumed constraint], what new approach becomes possible? State what would need checking before using it.

## Clarify

We're not sure what we're solving.

### tg-002 - FOG AHEAD

Revision 1 | Draft | Preserved V1 card | About 4 minutes

**Bearing:** Ask AI to ask you one clarifying question at a time.

**Ask the team:** Which unanswered question would most change our next step?

**Try it:** Give a short, non-sensitive challenge description. Answer up to three clarifying questions, one at a time. Stop and rewrite the challenge using what changed; do not let questioning replace action.

**Optional AI prompt:** Help clarify this challenge: [challenge]. Ask one question at a time, starting with the answer most likely to change the approach. After three questions, help me restate the challenge without inventing missing facts.

### tg-009 - MAP THE GAPS

Revision 2 | Draft | Preserved V1 card | About 4 minutes

**Bearing:** Ask AI to separate known facts from its assumptions.

**Ask the team:** What did we supply as evidence, and what is being filled in?

**Try it:** Take one AI answer. Mark claims grounded in the supplied material, assumptions and unanswered questions. Have a person check those labels against the original material; AI's own labels are not verification.

**Optional AI prompt:** Using only this supplied material: [material], separate supported claims, assumptions and open questions in this answer: [answer]. Quote short supporting passages where available. Say when support is missing.

**Situation tags:** ai-sounds-convincing

**Agent Workflow - Read the terrain**

**Agent prompt:** Inspect this repository before editing. Explain the relevant files, active project instructions, existing behavior and uncertainties. Cite file paths. Do not change files or execute project scripts yet.

**Human check:** A person can identify the relevant files and assumptions; no edits or project-script execution occurred.

**Codex note:** Ask Codex to identify the applicable AGENTS.md guidance and any overrides before proposing changes.

**Claude Code note:** Ask Claude Code which project instructions are active. Check the installed version and settings rather than assuming how CLAUDE.md and AGENTS.md interact.


### tg-010 - SHOW THE TARGET

Revision 1 | Draft | Preserved V1 card | About 3 minutes

**Bearing:** Give AI an example of what a good answer looks like.

**Ask the team:** What makes this example useful rather than merely polished?

**Try it:** Use a non-sensitive example of a useful output. Name two qualities to preserve and one thing not to copy. Ask AI to apply the qualities to a new task, then check the result against them.

**Optional AI prompt:** Here is a non-sensitive example: [example]. For [new task], preserve these qualities: [qualities], but do not copy [detail]. Produce a new answer and flag missing inputs.

### tg-016 - NAME THE DESTINATION

Revision 1 | Draft | New card | About 3 minutes

**Bearing:** State who needs what outcome, and how you will notice it helped.

**Ask the team:** Who is this for, and what should be different afterward?

**Try it:** Complete: For [role], we want [outcome], so they can [benefit]. Choose one observable sign of progress and one thing outside scope. Let the team correct the sentence before using AI.

**Optional AI prompt:** Help tighten this outcome statement: [statement]. Ask about ambiguous terms and suggest one observable sign of progress. Do not invent user needs or numerical targets.

**Situation tags:** too-many-ideas

### tg-017 - UNPACK THE WORD

Revision 1 | Draft | New card | About 3 minutes

**Bearing:** Pick one vague word in the request. Replace it with an observable requirement.

**Ask the team:** What would better, faster or useful look like in practice?

**Try it:** Circle one vague word in the task. Ask two people to explain what they mean by it, with passing allowed. Agree on a concrete example and a non-example, then rewrite the request.

**Optional AI prompt:** In this request: [request], identify one ambiguous term. Suggest two possible interpretations and ask us which we mean. Wait before rewriting the request.

## Experiment

We need something small to try.

### tg-004 - SMALL STEPS

Revision 2 | Draft | Preserved V1 card | About 10 minutes

**Bearing:** Ask AI for a test you can finish in ten minutes.

**Ask the team:** What can we learn with a small, safe test rather than a full build?

**Try it:** Choose one uncertain assumption. Define a reversible test with an observable result and a stop condition. Use synthetic or approved material, run the test, and record what it did and did not show.

**Optional AI prompt:** Propose a safe, reversible test of [assumption] that takes at most ten minutes using synthetic data. State the steps, observable result and stop condition. Do not treat a small test as proof of general success.

**Situation tags:** out-of-time

**Agent Workflow - Set the boundary**

**Agent prompt:** Implement only [agreed step] within [allowed scope]. Leave unrelated files alone. Ask before adding dependencies, changing configuration, accessing external services or taking destructive actions. Stop if the boundary prevents a correct solution.

**Human check:** The diff matches the agreed step, and scope changes were raised rather than silently included.

**Codex note:** Inspect the actual Codex sandbox and approval settings before starting. A narrow prompt supplements technical controls; it does not replace them.

**Claude Code note:** Inspect Claude Code permissions before starting. Keep action approvals appropriate to the exercise rather than disabling safeguards to avoid interruptions.


### tg-006 - FALSE SUMMIT

Revision 1 | Draft | Preserved V1 card | About 5 minutes

**Bearing:** Ask: How could this fail? Test one failure mode.

**Ask the team:** Which plausible failure could we safely expose now?

**Try it:** List possible failures and choose one low-risk case. Test with synthetic input in a sandbox or on paper. Stop if testing would affect real users, private data or live systems.

**Optional AI prompt:** For [idea], list three plausible failure modes. Propose a small sandbox test of one using synthetic inputs. Include a stop condition and avoid actions against real users or live systems.

**Situation tags:** ai-sounds-convincing

### tg-018 - ONE VARIABLE

Revision 2 | Draft | New card | About 5 minutes

**Bearing:** Change one part of a prompt. Compare the outputs against the same criterion.

**Ask the team:** What changed, and can we reasonably connect it to this one difference?

**Try it:** Keep the task and input fixed. Change one prompt instruction, compare results against one agreed criterion and note variability. One comparison is a clue, not a reliable performance estimate.

**Optional AI prompt:** For this task: [task], propose two prompts differing in exactly one instruction. Name the difference and one criterion for comparing their outputs. Do not predict a winner.

**Agent Workflow - Compare the approach**

**Agent prompt:** In your separate practice copy, solve [small task] against [shared acceptance checks]. Stay within [scope]. Report the diff and executed checks. Do not edit another agent's working copy or commit and push without approval.

**Human check:** Both attempts start from the same baseline in separate copies. Compare observable results, trade-offs and review effort, not writing style.

**Codex note:** Use a separate practice copy for Codex. Record the selected version and model locally so comparison limits are visible.

**Claude Code note:** Use a separate practice copy for Claude Code with the same baseline and task. Different settings and models limit what a single comparison can establish.


### tg-019 - TRAIL SAMPLE

Revision 1 | Draft | New card | About 6 minutes

**Bearing:** Try the same task on an ordinary, an edge and an incomplete example.

**Ask the team:** Does our idea still help when the input is awkward or incomplete?

**Try it:** Prepare three synthetic examples: ordinary, edge case and missing important information. Decide expected behavior before trying them. Check whether the system asks for missing information rather than guessing.

**Optional AI prompt:** Create three clearly synthetic inputs for [task]: ordinary, edge case and incomplete. For each, describe the behavior a human reviewer should look for, including when clarification is needed.

### tg-020 - PAPER BRIDGE

Revision 1 | Draft | New card | About 5 minutes

**Bearing:** Act out a workflow on paper before connecting tools or automating it.

**Ask the team:** Where should a person review or stop this workflow?

**Try it:** Sketch the input, proposed AI step, human review and final action. Walk through one synthetic example. Mark where permission, correction or stopping is needed before building any integration.

**Optional AI prompt:** For [workflow], sketch a paper prototype with input, AI suggestion, human review and final action. Identify a permission gate and a safe stopping point. Do not execute any actions.

## Verify

Can we trust this?

### tg-003 - CHECK BEARINGS

Revision 2 | Draft | Preserved V1 card | About 5 minutes

**Bearing:** Which claims need checking? Verify one independently.

**Ask the team:** Which claim would matter most if it were wrong?

**Try it:** Select one consequential factual claim. Check the original source, direct observation or a trusted reference independently of AI. If you cannot check it now, label it unverified and do not use it to justify a risky action.

**Optional AI prompt:** From this answer: [answer], identify consequential factual claims and suggest how a person could verify each independently. Do not invent citations, sources or verification results.

**Situation tags:** ai-sounds-convincing

**Agent Workflow - Show the evidence**

**Agent prompt:** Run the agreed, reviewed checks using [approved commands]. Report each command, observed result and any checks not run. Distinguish test evidence from assumptions and stop for approval if a command exceeds scope.

**Human check:** The team can see real check results, including failures and gaps, rather than only a confident completion summary.

**Codex note:** Ask Codex to distinguish commands it executed from commands it merely recommends. Independently inspect consequential results.

**Claude Code note:** Ask Claude Code to distinguish executed checks from suggested checks. Missing dependencies or denied permissions are limitations, not passing tests.


### tg-021 - FOLLOW THE SOURCE

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Open one cited source. Check that it exists and supports the actual claim.

**Ask the team:** Does this source support these words, or only a nearby topic?

**Try it:** Inspect an original cited source, including its context and applicability. Compare it with the exact claim. If unavailable, mark the claim unverified; do not accept a convincing-looking citation alone.

**Optional AI prompt:** Using only this supplied source excerpt: [excerpt], assess support for this claim: [claim]. Separate supported, unsupported and uncertain parts. Do not invent missing source content.

**Situation tags:** ai-sounds-convincing

### tg-022 - FIND THE EXCEPTION

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Look for a counterexample before treating an attractive claim as a rule.

**Ask the team:** What plausible case would make this claim fail?

**Try it:** Rewrite the claim so it can be checked. Suggest a counterexample and test it against supplied evidence or a safe direct check. If it fails, narrow the claim; if it survives, do not call that proof.

**Optional AI prompt:** For this claim: [claim], suggest a plausible counterexample and what evidence would test it. Clearly label hypothetical examples. Do not claim the test has been run.

**Situation tags:** ai-sounds-convincing

### tg-023 - MEASURE TWICE

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Recheck one number with a calculator or code. Include units and assumptions.

**Ask the team:** Are the inputs, units and calculation all correct?

**Try it:** Select a consequential number. Trace its inputs, recalculate independently using a calculator or code, and check units and rounding. A matching calculation cannot rescue unreliable inputs.

**Optional AI prompt:** For this numerical claim: [claim], list the inputs, units, assumptions and calculation needed for an independent check. Mark missing inputs. Do not claim calculator verification unless a tool was actually used.

**Situation tags:** ai-sounds-convincing

### tg-024 - SAFE TO SHARE

Revision 2 | Draft | New card | About 3 minutes

**Bearing:** Before submitting a prompt, remove private details and use synthetic examples where possible.

**Ask the team:** What information does this task really require us to share?

**Try it:** Review the proposed input locally before sending anything. Remove identifying, confidential and unnecessary details; follow the tool's approved-use policy. If unsure, use a synthetic example or do not submit it.

**Optional AI prompt:** Help with this clearly synthetic example: [synthetic example]. Do not request real names, credentials, private records or confidential material. Explain what additional non-sensitive context would help.

**Agent Workflow - Permission pause**

**Agent prompt:** Before taking [proposed action], explain what it reads or changes, any external access, the scope of permission needed and how we could recover. Wait for a decision. Do not reveal credentials or read private files to justify the request.

**Human check:** The human understands the proposed side effects and approves only the intended scope; refusal does not trigger a workaround.

**Codex note:** Review the actual Codex request and configured controls. Do not solve a denied action by switching to an unrestricted mode.

**Claude Code note:** Read the actual Claude Code permission request before approving. If denied, choose a permitted alternative or stop; do not bypass the control.


## Reflect

What did we learn?

### tg-008 - LEAVE MARKERS

Revision 1 | Draft | Preserved V1 card | About 3 minutes

**Bearing:** Save a useful prompt. Explain why it helped.

**Ask the team:** Which part of this prompt made the result more useful?

**Try it:** Save a sanitized prompt in an approved team workspace. Note the task, the useful instruction and where it might not transfer. Team Trail can mark the card used; it is not a free-text prompt notebook.

**Optional AI prompt:** Review this sanitized prompt and result: [prompt and result]. Suggest which instruction may have helped and where reuse may fail. Label causal explanations as hypotheses, not established facts.

### tg-011 - MAKE CAMP

Revision 2 | Draft | Preserved V1 card | About 3 minutes

**Bearing:** Summarize progress, open questions, and the next step.

**Ask the team:** What can the next person pick up without repeating our whole discussion?

**Try it:** Say or write three bullets: progress supported by evidence, open questions and the next concrete action. Name an owner for the next action with their agreement. Keep private discussion out of shared summaries.

**Optional AI prompt:** Using only these sanitized notes: [notes], draft three bullets: evidenced progress, open questions and next action. Mark missing information rather than inventing decisions or owners.

**Situation tags:** out-of-time

**Agent Workflow - Leave a handoff**

**Agent prompt:** Draft a concise handoff: changes made, checks actually run and results, remaining uncertainties, and the next safe action. Do not include secrets or personal details. Do not write, commit or push it without approval.

**Human check:** Another person can resume using evidence and explicit limits without reconstructing the entire chat.

**Codex note:** Keep stable project rules in the appropriate instruction file and task-specific progress in a separate handoff. Do not turn one-off context into permanent rules.

**Claude Code note:** Keep reusable project instructions separate from temporary task notes. Do not assume another person or tool shares the current conversation or private memory.


### tg-025 - SURPRISE ON THE TRAIL

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Compare what you expected with what happened. Name one surprise.

**Ask the team:** What surprised us enough to change our understanding?

**Try it:** Each person may name an expectation and an observation, with passing allowed. Separate what happened from your explanation for it. Choose one uncertainty to investigate rather than forcing a lesson.

**Optional AI prompt:** From these sanitized observations: [observations], separate expected result, observed result and possible explanations. Keep explanations tentative and propose one question for further investigation.

### tg-026 - KEEP OR ADAPT

Revision 2 | Draft | New card | About 4 minutes

**Bearing:** Name one move to keep, one to change and one thing you still cannot conclude.

**Ask the team:** What is worth repeating, and what should change next time?

**Try it:** Review one completed attempt. Name a useful move, a specific adjustment and a limit of the evidence. If using Team Trail later, attach feedback only to the attempt actually recorded, not the team's overall performance.

**Optional AI prompt:** Based only on these observations: [observations], suggest one practice to keep, one adjustment and one conclusion the evidence does not support. Avoid rating the people or team.

**Agent Workflow - Inspect the footprints**

**Agent prompt:** Review the current diff against [agreed scope] without editing. Explain changed files, unexpected changes, risks and unverified behavior. Point to concrete locations for human inspection. Do not discard any changes.

**Human check:** A human reviews the diff and can explain important behavior changes; generated explanations are checked against the actual changes.

**Codex note:** Ask Codex for a read-only diff review. A second review is additional scrutiny, not independent proof of correctness.

**Claude Code note:** Ask Claude Code for a read-only diff review. If comparing with another agent, preserve the same diff and acceptance criteria.


### tg-027 - TEACH THE NEXT GUIDE

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Explain one useful lesson to a teammate in plain language. Invite a correction.

**Ask the team:** Could someone else use this lesson without our original context?

**Try it:** One volunteer explains a technique using a synthetic example. A teammate restates it and asks a question; passing is welcome. Notice what needs clarification and revise the explanation together.

**Optional AI prompt:** Turn this lesson: [lesson], into a short explanation for a teammate, with a synthetic example and one limit. Avoid jargon and do not invent results we have not observed.

## Unstick

We're going in circles.

### tg-007 - TRAVEL LIGHT

Revision 1 | Draft | Preserved V1 card | About 3 minutes

**Bearing:** Try a fresh chat with only the essential context.

**Ask the team:** What is the smallest context someone needs to help us?

**Try it:** Write a compact task brief with the goal, necessary constraints and one sanitized example. Start a fresh chat using that brief. Keep safety requirements; do not drop essential context merely to shorten it.

**Optional AI prompt:** Help with [goal]. Necessary constraints: [constraints]. Sanitized example: [example]. Ask about missing essentials rather than guessing. Begin with one next step.

### tg-012 - CHANGE COURSE

Revision 1 | Draft | Preserved V1 card | About 4 minutes

**Bearing:** Ask AI for the strongest argument against your plan.

**Ask the team:** What would a thoughtful critic challenge first?

**Try it:** Describe the plan and ask for a substantive objection, not an attack on people. Identify which concern has evidence and which is hypothetical. Choose whether it calls for a change or a test.

**Optional AI prompt:** Give the strongest good-faith objection to this plan: [plan]. Separate evidence from assumptions and suggest a low-risk way to investigate the concern. Critique the plan, not the people.

### tg-028 - NAME THE BLOCKER

Revision 1 | Draft | New card | About 3 minutes

**Bearing:** Decide whether you lack information, a decision or a workable next action.

**Ask the team:** What is stopping us right now, in one sentence?

**Try it:** Each person may name the blocker. Agree whether the next move is to obtain information, ask a decision-maker or reduce the task. Pick one action that addresses that blocker instead of generating more ideas.

**Optional AI prompt:** Here is where we are stuck: [description]. Suggest whether the main gap is information, a decision or an actionable step. Explain uncertainty and ask one question before suggesting a move.

**Situation tags:** too-many-ideas, out-of-time

### tg-029 - SHRINK THE CLIMB

Revision 1 | Draft | New card | About 5 minutes

**Bearing:** Replace the whole problem with one useful piece you can finish now.

**Ask the team:** What is the smallest piece that would still move us forward?

**Try it:** Write the full task, then cross out everything not needed for one useful output. Pick a small deliverable, not a vague intention. Agree what done means and stop when it is met.

**Optional AI prompt:** Break this task: [task], into a smallest useful deliverable we can finish in five minutes. Give a clear done condition and what is deliberately out of scope. Do not omit essential safety checks.

**Situation tags:** out-of-time

### tg-030 - CHANGE THE MEDIUM

Revision 1 | Draft | New card | About 3 minutes

**Bearing:** Stop editing the prompt. Sketch, sort or act out the problem instead.

**Ask the team:** What becomes clearer if we step away from the chat window?

**Try it:** Choose paper, spoken explanation or movable notes. Represent the problem without AI for two minutes, then name one thing you could not see in the chat. Return to AI only if it serves a clear next step.

**Optional AI prompt:** Suggest a two-minute, no-AI sketching or sorting activity for this generic challenge: [challenge]. Keep it accessible and offer a spoken alternative. Do not ask for confidential details.

## Include

We need more voices involved.

### tg-031 - QUIET FIRST

Revision 1 | Draft | New card | About 3 minutes

**Bearing:** Give everyone quiet thinking time before discussion or AI suggestions.

**Ask the team:** What do we each notice before seeing someone else's answer?

**Try it:** Allow one minute for private thinking, writing or drawing. Invite sharing without requiring it, then group contributions before asking AI. Offer a spoken or paired alternative for anyone who prefers it.

**Optional AI prompt:** Using these voluntarily shared, sanitized ideas: [ideas], group similar points without ranking contributors. Preserve distinctive ideas and ask before merging points whose meanings differ.

**Situation tags:** nobodys-talking

### tg-032 - MAKE ROOM

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Offer each person a turn, with an explicit option to pass or contribute another way.

**Ask the team:** What would you like to add, question or pass on?

**Try it:** Invite a brief round without interruptions. State that passing is welcome and private or paired contributions are alternatives. Reflect back ideas without attaching judgments to who spoke or stayed quiet.

**Optional AI prompt:** Suggest a neutral invitation for a short discussion round about [generic topic]. Explicitly allow passing and written or paired alternatives. Do not pressure anyone to explain why they pass.

**Situation tags:** nobodys-talking

### tg-033 - PAIR BEFORE PLENARY

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Let people test an idea with a partner before offering it to the whole team.

**Ask the team:** What feels easier to explore in a pair first?

**Try it:** Offer pairs, solo reflection or a small group. Give a simple question and time to think. Invite volunteers to share a point, with permission before sharing a partner's contribution.

**Optional AI prompt:** For this generic topic: [topic], propose one clear pair-discussion question and a solo alternative. Keep the question low-pressure and do not request personal disclosures.

**Situation tags:** nobodys-talking

### tg-034 - MISSING VOICE

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Name an affected perspective that is absent. Record a question, not an invented answer.

**Ask the team:** Who might be affected but is not represented in this discussion?

**Try it:** Name an absent role or access need without naming a person. Write a question for someone with that perspective and mark related assumptions untested. AI role-play cannot substitute for their participation.

**Optional AI prompt:** For this idea: [idea], suggest potentially affected roles and questions to ask them. Do not speak on their behalf, infer demographic preferences or present simulated responses as actual feedback.

### tg-035 - PASS THE CONTROLS

Revision 2 | Draft | New card | About 3 minutes

**Bearing:** Offer a new driver and a reviewer. Keep participation optional and accessible.

**Ask the team:** Would another role or way of contributing help someone participate?

**Try it:** Offer roles such as prompt driver, questioner, reviewer or observer. Invite a voluntary swap and agree when to check in. Do not assume everyone wants to type, speak publicly or use the same interface.

**Optional AI prompt:** Suggest complementary roles for a small team doing [generic task] with AI. Include a non-typing role and an observer option. Keep role changes voluntary and avoid judging individual ability.

**Situation tags:** nobodys-talking

**Agent Workflow - Rotate the human roles**

**Agent prompt:** For [small task], suggest a driver, reviewer and questioner role, with an observer or non-typing alternative. Keep participation voluntary. The people, not the agent, decide who takes a role and who approves actions.

**Human check:** More than one person has an accessible way to contribute; driving the CLI is not treated as the only valuable role.

**Codex note:** The human driving Codex narrates the task and approval requests; another person checks the diff and evidence.

**Claude Code note:** The human driving Claude Code narrates the task and approval requests; rotate roles by agreement rather than making tool ownership permanent.


## Decide

We need to choose and move.

### tg-036 - CHOOSE THE COMPASS

Revision 2 | Draft | New card | About 4 minutes

**Bearing:** Agree on two criteria before comparing options.

**Ask the team:** What should matter most in this choice?

**Try it:** Name two criteria linked to the team's outcome. Separate mandatory safety or policy constraints from preferences. Compare options using available evidence and leave unknowns visible instead of inventing scores.

**Optional AI prompt:** Compare [options] using these agreed criteria: [criteria], and only this evidence: [evidence]. Mark unknowns and non-negotiable constraints. Do not invent numerical scores or make the decision for us.

**Situation tags:** too-many-ideas

**Agent Workflow - Agree the route**

**Agent prompt:** Propose a small plan for [task]. List likely files, scope boundaries, acceptance checks and unresolved questions. Do not edit or run project scripts yet. Wait for our agreement before implementation.

**Human check:** The team agrees on a bounded task, what is out of scope and observable acceptance checks.

**Codex note:** Give Codex the same written acceptance criteria the team will use to review its changes.

**Claude Code note:** Give Claude Code the same written acceptance criteria; a plan is a proposal, not permission to execute every later action.


### tg-037 - TWO TRAILS

Revision 1 | Draft | New card | About 4 minutes

**Bearing:** Shortlist two options. Name what you gain and give up with each.

**Ask the team:** Which trade-off are we actually willing to accept?

**Try it:** Use agreed criteria to narrow the list to two plausible options. For each, name a benefit, a cost and an uncertainty. Check dissenting concerns before choosing; do not keep generating alternatives by default.

**Optional AI prompt:** For these two options: [options], summarize a benefit, a cost and an uncertainty for each using [supplied evidence]. Do not add unsupported facts or choose on behalf of the team.

**Situation tags:** too-many-ideas

### tg-038 - REVERSIBLE STEP

Revision 2 | Draft | New card | About 4 minutes

**Bearing:** Choose a low-risk next step you can undo. Agree when to reconsider it.

**Ask the team:** Can we learn by moving without locking ourselves into this choice?

**Try it:** Check authority, safety and reversibility first. If the step is low-risk, agree an owner, a stopping rule and a review point. If consequences are significant or hard to reverse, seek appropriate review instead.

**Optional AI prompt:** For this possible next step: [step], list questions about reversibility, authorization, risks and stopping conditions. Suggest a limited trial only if the stated constraints allow one. Do not execute actions.

**Situation tags:** too-many-ideas, out-of-time

**Agent Workflow - Practice recovery**

**Agent prompt:** In a disposable practice copy, first inspect existing changes and propose one harmless edit plus a scoped recovery plan. Wait for agreement. Never overwrite unrelated work, run broad cleanup or use a destructive reset as a shortcut.

**Human check:** The team can identify and undo only the approved practice change while preserving pre-existing work. A checkpoint or commit alone is not a complete backup.

**Codex note:** Before Codex edits, inspect repository status and agree on a safe checkpoint. Have a human review the scoped recovery action before it runs.

**Claude Code note:** Before Claude Code edits, inspect repository status and agree on a checkpoint. Verify the actual recovery result rather than assuming conversation recovery restores every file or external action.


### tg-039 - NAME THE OWNER

Revision 1 | Draft | New card | About 3 minutes

**Bearing:** Turn the choice into one next action with an agreed owner and a done condition.

**Ask the team:** Who has agreed to do what, and how will we know it is done?

**Try it:** State the next action plainly. Invite an owner to agree, define the visible result and identify any needed permission. Put follow-up details in an approved team workspace; Team Trail does not yet store free-text commitments.

**Optional AI prompt:** Turn this sanitized decision: [decision], into a proposed next-action statement with placeholders for owner, done condition and required permission. Do not assign people or invent their agreement.

**Situation tags:** out-of-time

### tg-040 - CLOSE THE LOOP

Revision 1 | Draft | New card | About 3 minutes

**Bearing:** Say what you chose, why, and what new evidence would make you revisit it.

**Ask the team:** Can everyone tell what was decided and what is still open?

**Try it:** Summarize the choice and its rationale in plain language. Invite correction, surface unresolved concerns and name a reconsideration trigger. Distinguish an agreed decision from a proposal awaiting permission.

**Optional AI prompt:** Using these sanitized notes: [notes], draft a decision summary with rationale, open concerns and a reconsideration trigger. Mark proposals separately from agreed decisions. Do not infer consensus from silence.

**Situation tags:** out-of-time

## Maintenance and validation

Edit content/cards.json as the source of truth and regenerate this Markdown view in the same change. Preserve IDs, increment changed content revisions, and run the content contract test. The draft review state is not evidence of facilitator approval.

```sh
python3 -m unittest discover -s tests -p content_test.py -v
```

The tests check theme counts, stable IDs, required fields, situation tags, verbatim preservation of the twelve V1 actions and presence of content in this catalog. They do not establish editorial quality, runtime integration or hardware readability.
