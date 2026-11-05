# Deep Coaching content review — TrailGuide v3-0.1.21

## Executive assessment

**The collection has a coherent, humane foundation. The best next step is a targeted editorial and retrieval pass—not a replacement deck, more mandatory steps, or a curriculum built around the device.**

The strongest features are open invitations, room for human-only work, explicit permission to pass or request direct assistance, meaningful distinctions among the six paths, and explicit reminders that AI self-critique is not evidence. The content generally leaves choices with the learner rather than positioning the facilitator or AI as the decision maker.

The largest opportunity is **making the short Coaching collection carry enough of the useful support already present elsewhere**. Some referenced experiments import only a polite invitation, not the canonical activity or checking method. Guide me often repeats a path-wide stance rather than helping with that particular question or experiment. A few questions assume a disappointing result, prior discoveries, a shared team view or a conclusion that has not yet been established.

This is an editorial/facilitation assessment, not evidence of poor outcomes or a validated learning intervention. The proposed changes below are unapproved examples and discussion priorities. **No card wording, generated artifact, review state, firmware, UI, device or publication state was changed.**

## Scope and evidence

The review covers:

- **55 Coaching entries:** six openers, 24 questions, 18 optional experiments, three closings and four private reminders.
- All visible text and every supplied private-guidance field, not just card titles.
- **40 canonical cards**, including their Ask/Try/activity/guidance relationships and optional adaptations.
- The distinct **12-card Classic/V3 selection** and **11 exact Coaching text references**.
- Six-path differentiation, independent/random entry, varied individual/team contexts, AI-assisted checking, privacy/participation boundaries, and learner-owned continuation.

Five focused reviews were combined with programmatic coverage, reference-resolution and source-integrity checks. The two native content generators passed read-only freshness checks. This does not constitute a fresh rendering test or a field trial.

All 55 Coaching records remain **draft**. Physical approval of the device or a preference for the V3 wording does not silently promote editorial review states. The 40-card catalog, 12-card Classic selection and 55 Coaching faces remain related but distinct resources—not one merged deck.

### How to read the companion audits

- [Every Coaching entry](coaching-v3-0.1.21/COACHING-CARD-AUDIT.csv): 55 rows with exact current prompt/guidance, purpose, strengths, conditional limitations, individual/team notes, checking considerations and proposed validation.
- [Every canonical card](coaching-v3-0.1.21/CANONICAL-CARD-AUDIT.csv): 40 rows, Classic membership, Coaching consumers, relationship audit, adaptations and recommendations.
- [Shared-text crosswalk](coaching-v3-0.1.21/SHARED-TEXT-CROSSWALK.csv): all 11 references, imported fields and change consequences.
- [Selective draft examples](coaching-v3-0.1.21/DRAFT-EXAMPLES.md): illustrations for discussion, not a replacement deck.
- [Loop map and hypothetical walkthroughs](coaching-v3-0.1.21/LOOP-AND-SCENARIO-MAP.md): how existing resources can support different situations without becoming an ordered program.

The Coaching dispositions are 17 **keep**, 25 **clarify**, 11 **refine** and two **discuss**. These are editorial notes, not quality scores or approval decisions. “Clarify” often means a contextual facilitation cue, not that the visible question must be rewritten. The reviewers produced 45 overlapping observations, risks and ideas; they are consolidated below into **ten thematic priorities**, not presented as 45 defects.

## Review principles

1. Start from personally meaningful work and the kind of assistance wanted—not a requirement to use AI.
2. Preserve **Define → Direct → Check → Adjust** as the reusable learner-facing loop. Plan an adequate check during Define; do not print a second mandatory sequence on every card.
3. Let requested explanation, demonstration or direct assistance happen. Questions and experiments are options, not entry requirements for help.
4. Keep passing, adaptation, stopping and human-only work complete choices. Do not diagnose silence, infer agreement, label ability or equate greater AI use with progress.
5. Let AI and tools do useful checking, anchored to sources, observations, calculations, tests or purpose-specific criteria. Neither model agreement nor exhaustive human rechecking is the default standard.
6. Compare preparation, direction, checking, correction, integration, attention and learning value when route choice warrants it—not generation speed alone.
7. Support independent continuation without equating independence with abandonment or exclusive reliance on another AI.
8. Treat card draws, views and completed activities as neither learning evidence nor consent/outcome records.

These principles primarily reflect the project’s stated purpose and the actual content. As a limited external lens, UNESCO emphasizes a human-centred mindset, ethics and critical judgement of AI solutions.[1] Its framework is school-oriented; it does not establish effectiveness for these adult/project mentoring materials. NIST’s generative-AI profile identifies risks including confabulation, privacy and problematic human-AI configurations such as over-reliance.[2] That supports checking the basis and boundaries of AI help, not adding a risk checklist to every conversation.

## What is already strong

### Permission is substantive, not merely a polite opening

Experiments explicitly permit adaptation, declining, direct help or another next step. Stuck guidance specifically says not to withhold requested assistance to prolong questioning. Exploring recognizes human-only work as a complete choice. Team guidance permits passing and multiple contribution formats, allows differences to remain visible, and rejects diagnosing silence.

These are important strengths to preserve during shortening. Do not remove safeguards merely because identical words occur on several independently accessible cards.

### Checking is already more mature than “ask another model”

`PROGRESS-E02` asks for consequential claims to be checked against evidence. `AI-RESULTS-E01` names sources, observations and executable checks. Every experiment’s guidance says AI self-critique is not proof and rejects exhaustive human auditing. Canonical `tg-003`, `tg-021` and `tg-023` provide practical evidence/checking resources.

The issue is uneven retrieval and task-specific detail, **not an absence of checking or a blanket instruction to trust AI**.

### The six paths have different jobs without becoming learner categories

Progress sustains what is useful; Stuck investigates an obstacle; Unclear clarifies purpose and uncertainty; Exploring opens alternatives; AI Results examines output fitness; Team makes perspectives and collaboration discussable. Their overlap is useful. More than one can fit the same project situation, and no routing quiz or “correct” path is needed.

### The wider collection already contains bridges to independence

Human-only comparison, portable context, handoff, review, reconsideration and a learner-owned continuation brief already exist. `docs/PROJECT-COACH.md` explicitly supports later human or AI help and says independence is not abandoning support. A finding that the project has no continuation resource would be false.

The opportunity is making those resources easier to retrieve from a short closing or standalone Coaching interaction.

## Ten thematic priorities

Priorities describe recommended discussion/work order, not safety severity or permission to implement. Each includes a small useful check rather than an exhaustive manual review requirement.

### CR-01 — Make Guide me more specific to the selected face

**Observation:** 48 records contain private guidance, but there are only 14 distinct guidance texts. Each path repeats one paragraph across its four questions and an expanded paragraph across its three experiments. The longest experiment guidance is 63 words. These are raw text measurements, not readability or fit scores.

**Why it matters:** a stance such as “do not invent agreement” is valuable, but it may not help the facilitator decide what to listen for in this particular question, what to notice in this experiment, or how to judge an AI criticism. Identical AI Results guidance serves critiquing an output, checking one claim and comparing criteria, even though those tasks need different support.

**Recommendation:** selectively substitute one face-specific listening or checking cue where the generic introduction adds least value. Retain accessible choice/evidence safeguards on independent entry. Avoid simply appending more text, reading private guidance aloud as a lecture, or hiding every safeguard in a remote global reminder.

**Evidence:** the question/experiment guidance families in `content/coaching.json`; especially `AI-RESULTS-E01`–`E03`, `STUCK-E01`, `PROGRESS-Q01`–`Q04`.

**Validation:** with only the selected record, can a facilitator identify one relevant thing to notice or check while preserving direct-help/decline choices? Compare a small candidate against the existing text before changing the whole collection.

**Effort/tradeoff:** medium, selective editorial work. More specificity costs words; over-centralization can hide protections when a card is entered directly.

### CR-02 — Distinguish an experiment invitation from its method

**Observation:** four experiments reference canonical **Try invitations**, not their full activities:

- `STUCK-E01` → `tg-004.facilitator.try_invitation`.
- `EXPLORING-E01` → `tg-014.facilitator.try_invitation`.
- `AI-RESULTS-E02` → `tg-003.facilitator.try_invitation`.
- `TEAM-E01` → `tg-031.facilitator.try_invitation`.

Their local guidance does not automatically inherit canonical activity steps, specific listening/checking cues or precautions. For example, a small safe test invitation does not itself explain the observation, stopping boundary or limits of what one result can establish.

**Recommendation:** keep the concise invitations. Where a novice facilitator would otherwise need to invent the method, supply a small local run/listen/check cue or an explicitly reachable supporting example. Do not paste entire canonical activities onto the main face or make every reflective activity a controlled experiment.

**Validation:** inspect a harmless hypothetical use with only the Coaching record. Can someone choose an appropriate action, observation and stop point, or adapt/decline it? No real device or participant experiment was performed for this review.

**Effort/tradeoff:** small-to-medium for selected guidance. Method support improves independence, but excessive recipes can displace learner choices.

### CR-03 — Remove only the presuppositions that unnecessarily restrict entry

**Observation:** a few questions assume prior work, a discovery, useful output, multiple attempts or disappointment. Examples include `OPEN-03`, `OPEN-05`, `PROGRESS-Q03`, `AI-RESULTS-Q02` and `AI-RESULTS-Q04`. “What were you hoping to see instead?” is especially conditional: it suggests a mismatch even when the output is useful. A generic conditional follow-up in private guidance does not condition that main question itself.

**Recommendation:** first distinguish a useful context-specific question from an unnecessarily restrictive default. Adapt or skip a sound question where its context does not fit. Consider a small wording or applicability adjustment for the clearest cases; do not flatten all openings into the same maximally neutral sentence.

**Validation:** read the selected item as first contact, with no prior attempts, and with a good AI result. An honest response should not require inventing a discovery or defect. “Nothing yet” remains legitimate.

**Effort/tradeoff:** small, scoped changes. Lively progress/discovery questions remain useful in the right circumstances; excessive qualifications can make conversation hesitant or bureaucratic.

### CR-04 — Let a check change confidence, not merely strengthen it

**Observation:** `PROGRESS-E03` asks for an observation that would “strengthen confidence.” It can frame checking as reassurance-seeking. This is a textual asymmetry and an untested use risk—not evidence that users actually exhibit confirmation bias.

**Recommendation:** consider “change your confidence” or a similarly open formulation. Agree what different observations would mean before checking. Do not manufacture doubt when the learner simply wants to acknowledge sound progress.

**Validation:** can the facilitator describe an observation that would weaken confidence as well as one that would strengthen it, without demanding that both be collected?

**Effort/tradeoff:** small. Neutral checking is more informative, but it should remain proportionate rather than turning every success into an investigation.

### CR-05 — Make the evidence basis practical and appropriate to the task

**Observation:** the collection already protects against AI self-critique as proof. However, short invitations can leave the checking basis abstract, and an AI-generated criticism can itself be wrong or irrelevant.

**Recommendation:** reuse existing resources to offer a small optional evidence menu:

- A factual claim: a relevant source passage and its quality/context.
- A number or deterministic transformation: executed calculation/test, units and assumptions.
- Writing, design or explanation: purpose, audience, constraints and comparison with the supplied material.
- A proposed change: an observation or bounded reversible example when appropriate.
- An unavailable check: explicit uncertainty, a smaller claim, another route or suitable human expertise.

AI/tools can propose, locate and perform appropriate checks with inspectable results. Check consequential criticisms before spending effort fixing them. Neither another model nor a human’s complete re-audit is automatically required.

**Evidence:** `AI-RESULTS-E01`–`E03`, `PROGRESS-E02`, `tg-003`, `tg-009`, `tg-021`, `tg-023`.

**Validation:** for one actual type of project artifact, name a useful result, a suitable evidence basis and an adequate stopping point. Reflection or creative choice need not pretend to be a factual experiment.

**Effort/tradeoff:** medium, selected guidance/examples. A long evidence menu on every face would add a competing checklist.

### CR-06 — Compare complete routes when the choice matters

**Observation:** `EXPLORING-E01` imports the human-only invitation, while its canonical `tg-014` activity contains the fuller comparison of preparation, checking, correction and learning value. Broader workflow/integration support also exists. The short Coaching path can therefore make a comparison less complete than the linked catalog context.

**Recommendation:** retain human-only work as a full choice, not a detour before the “real” AI task. When useful, compare realistic human, AI-assisted and hybrid routes by direction/preparation, checking, correction, integration, attention and the capability the learner wants to practice. Do not force all routes to be tried or promise precise savings without evidence.

**Validation:** compare two plausible routes for one meaningful milestone. Include using the output and learning from the work, not just generating it. A simple task may not warrant a formal comparison.

**Effort/tradeoff:** small supporting cue or worked example; excessive comparison can cost more than the task.

### CR-07 — Keep participation voluntary under real power and privacy constraints

**Observation:** Team guidance strongly protects passing, alternatives and disagreement. `TEAM-Q04` nevertheless asks what “everyone” thinks, which can invite one person to speak for others. Private writing or pairs do not guarantee confidentiality or safety. `REMIND-04` imports an entire group activity, including inviting a round, rather than just a short reminder principle.

**Recommendation:** invite people’s own perspectives, allow no contribution to be collected, seek permission before sharing another person’s words, and leave disagreement visible. Where authority or disclosure pressure matters, an individual conversation, another route or stopping may be preferable. Do not promise anonymity or send private concerns to AI. Treat a round and temporary roles as optional methods, not requirements or learner labels.

**Validation:** in a hypothetical manager/team situation, can a person pass or retain a private reflection without a diagnosis or demand for an explanation? Can an individual use the intent without inventing teammates? These are desk scenarios, not observed coercion or a completed safety assessment.

**Effort/tradeoff:** small conditional guidance. Too much facilitation machinery can overcomplicate ordinary collaboration; too little can overstate what an invitation makes safe.

### CR-08 — Let closing include uncertainty, pausing and further help

**Observation:** the three closings are concise and learner-owned. As standalone early entries, however, “What’s your next step?” or “What would you like to try first?” can imply that a decision or experiment is due. “How will you know whether that helped?” presumes a referent that may need clarifying.

**Recommendation:** preserve the short questions. Give facilitators permission to establish the referent, accept “not yet,” pause, ask for support or stop without converting ownership into a commitment gate. Some context is necessary for useful help; a full diagnostic account or a fixed number of questions is not.

**Validation:** consider an unresolved project, someone asking for demonstration, and someone choosing to stop. Can the conversation close respectfully without inventing an action or evidence of learning?

**Effort/tradeoff:** low editorial/context support. Adding every possible exit to the visible text would weaken its conversational economy.

### CR-09 — Bridge to learner-owned continuation using what already exists

**Observation:** `docs/PROJECT-COACH.md`, portable-context/handoff cards and transfer activities already support continuation. The short Coaching closings do not directly surface all of that support. Independence is supported more clearly than a gradual handover of who defines, directs, checks and adjusts is made explicit.

**Recommendation:** offer the existing brief as an optional off-device resource, using only useful fields. If independence is an agreed goal, let the learner lead the next loop while the mentor reduces prompting. Later human or AI assistance remains legitimate. A next action and one unresolved check can be enough; no mandatory form, transcript, profile or device record is needed.

**Validation:** can someone retain useful context and choose their next help/check without this facilitator, while still knowing they may seek support? One successful supported exchange does not establish durable transfer.

**Effort/tradeoff:** low for a retrieval bridge; a detailed handoff can cost more than it saves.

### CR-10 — Govern shared fields deliberately; preserve distinct surfaces

**Observation:** all 11 references resolve faithfully, but each imports a specific field. Editing a canonical Ask can affect a Classic face and a Coaching question; editing canonical activity or Guide usually does not change a Coaching entry that references only the invitation. `REMIND-04` is the contrasting case that imports the complete activity.

**Recommendation:** use the crosswalk before edits. Prefer a local Coaching guidance change when that is the intended scope; review the broader effect before changing a shared field. Preserve IDs, qualifications, draft states and optional Agent Workflow adaptations. The ten-card agent collection is an optional cross-theme adaptation, not extra cards, a new theme, or the purpose of every card.

**Validation:** for each proposed edit, enumerate exact consumers, regenerate/check affected outputs later, and compare meaning as well as bytes. Source editing, generation, publication, building and installation are separate operations—not automatic consequences of a review.

**Effort/tradeoff:** low process overhead. Local adaptation can drift; indiscriminate reuse can put an activity where a private reminder or invitation was intended.

## Path-by-path assessment

### Progress

**Keep:** noticing success without manufacturing a problem; preserving useful work; building on a learner-chosen direction. The shared `tg-026` Ask is a good fit.

**Refine selectively:** questions about surprise need a matching history. Transfer assumes a successful approach is available. Keep the evidence requirement on AI challenge; consider balanced confidence-check wording in `PROGRESS-E03`.

**Meaningful result:** a learner can name what is useful to retain or explore next; no lesson, change or problem must be manufactured.

### Stuck

**Keep:** expected versus actual results, room for multiple explanations, explicit direct-help protection and reversible exploration.

**Refine selectively:** “What have you tried?” must allow “nothing yet,” access barriers and a request for demonstration. The small-test invitation needs a concrete observation/stop cue when used without canonical activity support. Optional blocker categories from the broader catalog must not become diagnoses.

**Meaningful result:** enough understanding for suitable assistance or a bounded next move—not a completed interrogation or proof of persistence.

### Unclear

**Keep:** learner-defined purpose, uncertainty, useful benefit and comparing success definitions before selecting a solution.

**Refine selectively:** “problem,” “solving” and “requirements” fit some work better than a poem, personal exploration or hobby. Use the learner’s language and allow spoken/sketched clarification. Choose a sufficient checking basis before production where helpful; no demand for perfect clarity first.

**Meaningful result:** a more useful question, purpose, constraint or next thing to learn; producing a formal problem statement is not inherently the goal.

### Exploring

**Keep:** contrasting approaches, no forced winner, explicit human-only work and a counterargument without requiring a change of mind.

**Refine selectively:** identify the ideas/criteria if none exist yet. The human-only invitation does not itself bring the canonical total-effort comparison into Coaching. An argument against an idea is a possibility to examine, not evidence that it is wrong.

**Meaningful result:** a useful distinction, question or chosen exploration—not a quota of alternatives or an AI trial.

### AI Results

**Keep:** output fitness rather than prompt tuning alone; goal/context/data/tool limitations; sources, observations and executable checking; criteria grounded in the task.

**Refine selectively:** the “instead” question assumes disappointment, the useful-example question assumes the example is useful, and retrospective questions assume prior attempts. Replace repeated generic Guide text with a relevant cue for claim checking, criteria comparison or critique. Verify consequential criticisms before acting on them.

**Meaningful result:** a supported judgment of fitness, a visible uncertainty, a justified change or a decision not to use the output—not a more elaborate prompt by default.

### Team

**Keep:** multiple participation formats, permission to pass, no diagnosis of silence and no invented consensus. Questions permit collaboration to be examined without an automatic fix.

**Refine selectively:** ask people for their own views rather than a proxy report for everyone. Distinguish understanding an experiment from permission to participate. Pair/private options need context and sharing boundaries; neither is a guarantee of safety.

**Meaningful result:** perspectives, differences or an explicitly agreed next arrangement become clearer; unanimity and public disclosure are not required.

## Openers, closings and reminders

- **Openers:** retain conversational variety. `OPEN-04` is particularly useful for a fresh purpose; `OPEN-01/02` suit current work. Discovery/surprise and team-direction openings are more conditional. A selective applicability cue may be cheaper than rewriting all six.
- **Closings:** keep the learner-owned brevity, but do not read them as a required action commitment or a three-question sequence. Clarify what “that” refers to and accept pausing/support/stopping.
- **Reminders:** the first three are concise private cues. `REMIND-04` preserves valuable passing/alternative/no-judgment language but carries a full group activity. If a shorter reminder is wanted, discuss a local adaptation rather than truncating the shared inclusion activity.

## Classic/V3 and the wider catalog

The revised 12-card Classic selection is not obsolete wording. Its paired Ask/Try/Guide structure often contains more concrete method and checking support than the short Coaching invitation. The broader 40-card collection adds audience, analogy, numerical/source checking, planning, collaboration, handoff and reconsideration. Those resources should be reused selectively rather than duplicated into an ever-growing Coaching catalog.

Conditional concerns include AI-first action fronts on otherwise voluntary Try activities, retrospective slots that can imply a lesson/change is due, and activities assuming another participant or prior criteria. These are contextual adaptation questions, not proof that the collection excludes individuals, creative work or human-only routes. The card audit records them individually.

The optional clarification AI template on `tg-002` also merits a small consistency review: the activity allows up to three questions and stopping when enough is clear, while the separate template says “after three.” That template is not the shipped `UNCLEAR-Q01` question. Preserve the early-stop intent without inventing a Coaching question quota.

## Suggested next increment — discussion only

1. Agree a small selection of questions/applicability cues to refine, starting with clear expectation/confirmation issues rather than rewriting all prompts.
2. Give the four invitation-based experiments appropriate local method support where needed.
3. Replace selected generic Guide introductions with one useful face-specific cue while preserving accessible permissions and evidence boundaries.
4. Create one optional off-device bridge to the loop, human/AI/hybrid route comparison and existing continuation resource.
5. Review the selected wording against its exact consumers and actual viewport before any implementation or editorial approval.

Use existing observations first. If a natural mentoring opportunity arises with permission, investigate one relevant uncertainty on one meaningful milestone. Observe whether the learner can choose a next useful result, a proportionate check, suitable assistance and a continuation route. Do not require new role-plays, exhaustive card trials, participant identities, transcripts, scoring or captured prompts.

**Stopping point:** resolve a material content uncertainty and revise only the affected support. Do not grow the collection merely to fill a theoretical coverage matrix. This review establishes neither learning gains nor a finished field validation.

## Sources

[1] https://www.unesco.org/en/articles/ai-competency-framework-students — UNESCO: AI competency framework for students
    > "critical judgement of AI solutions"
[2] https://nvlpubs.nist.gov/nistpubs/ai/NIST.AI.600-1.pdf — NIST AI600-1: Generative Artificial Intelligence Profile
    > "over-reliance, or emotional entanglement with GAI"
