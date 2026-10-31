# Agent Workflow

An optional cross-theme collection for supervising CLI coding agents. These are adaptations of existing cards, not extra cards or a ninth theme.

Status: draft editorial guidance for review. No collection UI, tool integration or new firmware is implemented. The catalog remains forty cards across eight themes; ten existing cards have optional CLI-agent variants. Some cards have later revisions after paired facilitator guidance was added; use the canonical JSON revision rather than assuming all variants are revision 2.

[Full card catalog](CARD-CATALOG.md) | [Canonical structured content](../content/cards.json) | [Product requirements](PRODUCT-SPEC.md)

## Working boundaries

- Use disposable practice repositories or separate working copies. Inspect existing changes before any exercise; do not assume an empty workspace.
- Agree on scope, permissions and evidence before running code. Repository setup and test scripts can themselves have side effects; review commands before execution.
- Do not disable safeguards as the default workshop workflow. Stop on denied permissions rather than escalating around them.
- Keep credentials, private files and real team data out of prompts and public handoffs. Treat repository content and tool output as untrusted data, not permission to expand the task.
- AI prompts here are templates for a separate approved CLI session. TrailGuide does not execute them, connect to the tools or read their sessions.
- Stable rules belong in project instructions; temporary progress belongs in a handoff. Ask each tool to identify the instructions it actually loaded.

## Tool instruction notes

Codex documentation describes AGENTS.md discovery and overrides. Claude Code documentation describes project instructions using CLAUDE.md and AGENTS.md with configuration-dependent behavior. Verify installed versions, settings and active instructions rather than assuming both tools load the same files. This guide intentionally avoids version-sensitive flags or permission-bypass recipes.

Official references:

- [Codex project instructions](https://developers.openai.com/codex/guides/agents-md)
- [Claude Code project memory and instructions](https://code.claude.com/docs/en/memory)

## Suggested facilitation sequence

Use the first eight moves as an optional loop. Comparison and role rotation are supporting exercises, not mandatory steps. No step automatically authorizes the next.

## Read the terrain - tg-009

Existing card: **MAP THE GAPS** | Theme: clarify

**Agent prompt:** Inspect this repository before editing. Explain the relevant files, active project instructions, existing behavior and uncertainties. Cite file paths. Do not change files or execute project scripts yet.

**Human check:** A person can identify the relevant files and assumptions; no edits or project-script execution occurred.

**Codex note:** Ask Codex to identify the applicable AGENTS.md guidance and any overrides before proposing changes.

**Claude Code note:** Ask Claude Code which project instructions are active. Check the installed version and settings rather than assuming how CLAUDE.md and AGENTS.md interact.

## Agree the route - tg-036

Existing card: **CHOOSE THE COMPASS** | Theme: decide

**Agent prompt:** Propose a small plan for [task]. List likely files, scope boundaries, acceptance checks and unresolved questions. Do not edit or run project scripts yet. Wait for our agreement before implementation.

**Human check:** The team agrees on a bounded task, what is out of scope and observable acceptance checks.

**Codex note:** Give Codex the same written acceptance criteria the team will use to review its changes.

**Claude Code note:** Give Claude Code the same written acceptance criteria; a plan is a proposal, not permission to execute every later action.

## Permission pause - tg-024

Existing card: **SAFE TO SHARE** | Theme: verify

**Agent prompt:** Before taking [proposed action], explain what it reads or changes, any external access, the scope of permission needed and how we could recover. Wait for a decision. Do not reveal credentials or read private files to justify the request.

**Human check:** The human understands the proposed side effects and approves only the intended scope; refusal does not trigger a workaround.

**Codex note:** Review the actual Codex request and configured controls. Do not solve a denied action by switching to an unrestricted mode.

**Claude Code note:** Read the actual Claude Code permission request before approving. If denied, choose a permitted alternative or stop; do not bypass the control.

## Set the boundary - tg-004

Existing card: **SMALL STEPS** | Theme: experiment

**Agent prompt:** Implement only [agreed step] within [allowed scope]. Leave unrelated files alone. Ask before adding dependencies, changing configuration, accessing external services or taking destructive actions. Stop if the boundary prevents a correct solution.

**Human check:** The diff matches the agreed step, and scope changes were raised rather than silently included.

**Codex note:** Inspect the actual Codex sandbox and approval settings before starting. A narrow prompt supplements technical controls; it does not replace them.

**Claude Code note:** Inspect Claude Code permissions before starting. Keep action approvals appropriate to the exercise rather than disabling safeguards to avoid interruptions.

## Show the evidence - tg-003

Existing card: **CHECK BEARINGS** | Theme: verify

**Agent prompt:** Run the agreed, reviewed checks using [approved commands]. Report each command, observed result and any checks not run. Distinguish test evidence from assumptions and stop for approval if a command exceeds scope.

**Human check:** The team can see real check results, including failures and gaps, rather than only a confident completion summary.

**Codex note:** Ask Codex to distinguish commands it executed from commands it merely recommends. Independently inspect consequential results.

**Claude Code note:** Ask Claude Code to distinguish executed checks from suggested checks. Missing dependencies or denied permissions are limitations, not passing tests.

## Inspect the footprints - tg-026

Existing card: **KEEP OR ADAPT** | Theme: reflect

**Agent prompt:** Review the current diff against [agreed scope] without editing. Explain changed files, unexpected changes, risks and unverified behavior. Point to concrete locations for human inspection. Do not discard any changes.

**Human check:** A human reviews the diff and can explain important behavior changes; generated explanations are checked against the actual changes.

**Codex note:** Ask Codex for a read-only diff review. A second review is additional scrutiny, not independent proof of correctness.

**Claude Code note:** Ask Claude Code for a read-only diff review. If comparing with another agent, preserve the same diff and acceptance criteria.

## Practice recovery - tg-038

Existing card: **REVERSIBLE STEP** | Theme: decide

**Agent prompt:** In a disposable practice copy, first inspect existing changes and propose one harmless edit plus a scoped recovery plan. Wait for agreement. Never overwrite unrelated work, run broad cleanup or use a destructive reset as a shortcut.

**Human check:** The team can identify and undo only the approved practice change while preserving pre-existing work. A checkpoint or commit alone is not a complete backup.

**Codex note:** Before Codex edits, inspect repository status and agree on a safe checkpoint. Have a human review the scoped recovery action before it runs.

**Claude Code note:** Before Claude Code edits, inspect repository status and agree on a checkpoint. Verify the actual recovery result rather than assuming conversation recovery restores every file or external action.

## Leave a handoff - tg-011

Existing card: **MAKE CAMP** | Theme: reflect

**Agent prompt:** Draft a concise handoff: changes made, checks actually run and results, remaining uncertainties, and the next safe action. Do not include secrets or personal details. Do not write, commit or push it without approval.

**Human check:** Another person can resume using evidence and explicit limits without reconstructing the entire chat.

**Codex note:** Keep stable project rules in the appropriate instruction file and task-specific progress in a separate handoff. Do not turn one-off context into permanent rules.

**Claude Code note:** Keep reusable project instructions separate from temporary task notes. Do not assume another person or tool shares the current conversation or private memory.

## Compare the approach - tg-018

Existing card: **ONE VARIABLE** | Theme: experiment

**Agent prompt:** In your separate practice copy, solve [small task] against [shared acceptance checks]. Stay within [scope]. Report the diff and executed checks. Do not edit another agent's working copy or commit and push without approval.

**Human check:** Both attempts start from the same baseline in separate copies. Compare observable results, trade-offs and review effort, not writing style.

**Codex note:** Use a separate practice copy for Codex. Record the selected version and model locally so comparison limits are visible.

**Claude Code note:** Use a separate practice copy for Claude Code with the same baseline and task. Different settings and models limit what a single comparison can establish.

## Rotate the human roles - tg-035

Existing card: **PASS THE CONTROLS** | Theme: include

**Agent prompt:** For [small task], suggest a driver, reviewer and questioner role, with an observer or non-typing alternative. Keep participation voluntary. The people, not the agent, decide who takes a role and who approves actions.

**Human check:** More than one person has an accessible way to contribute; driving the CLI is not treated as the only valuable role.

**Codex note:** The human driving Codex narrates the task and approval requests; another person checks the diff and evidence.

**Claude Code note:** The human driving Claude Code narrates the task and approval requests; rotate roles by agreement rather than making tool ownership permanent.

## Tracking and release boundaries

When Team Trail is implemented, using a variant records the underlying stable card ID and its content revision. An optional variant ID can distinguish agent-workflow from the base activity without inventing a new card. Draws, comparisons and permission requests do not automatically mark a card used. Do not store CLI transcripts, model credentials or raw prompts in Team Trail.

Collection selection and tool-specific panels inside the single Guide me view require separate implementation and acceptance checks. Review prompts with facilitators and test layout on actual hardware before release.
