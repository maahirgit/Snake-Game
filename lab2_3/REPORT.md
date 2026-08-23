# Lab 2_3 — Snake-Game (`maahirgit/Snake-Game`)

---

## 1. Tool and install route — [3]

| | |
|---|---|
| Agent used for run 2 | Claude Code (Opus 5), Windows 11, MinGW g++ 11.2.0 |
| `ubiquitous-language` install route | `~/.claude/skills/ubiquitous-language`, copied from the course repo |
| `refactoring/` pack install route | `~/.claude/skills/`, all ten folders copied from `skills/refactoring/*` |

Both installs worked on the first attempt; the paste fallback was not needed. The skills
were taken from a clone of `Ankush-Chander/IT643-software-design-and-testing`. One
correction to the assignment text: the report template and the checker are at
`docs/labs/lab2_3/` in that repository, not `lab2_3/`, so the `cp` and `curl` paths in the
handout do not resolve as written.

`ubiquitous-language` does set `disable-model-invocation: true` and has to be asked for by
name, as the handout warns.

The upstream `refactor` orchestrator was **not** installed and not used. Commits 3 and 4
were written directly, which is the point of keeping them separate.

---

## 2. What I changed in the glossary — [4]

The generated draft was corrected in six ways. It invented three terms out of the README's
"improvement ideas" list — Obstacle, Theme and Wrap-around — which name features that do
not exist; those were deleted, along with Round and Session, which nobody says. It made
**Fruit** canonical over **Food**, which inverts the evidence: the only human prose in this
repository is the README and it says food throughout, so Food is canonical and Fruit is now
the alias to avoid. It cited `Snake` as the code for **Player**, which is wrong — that is
evidence for Snake; Player has no representation at all, and saying so is the finding this
lab turns on. It defined **High Score** as an all-time best when `clearHighScoreFile()`
zeroes it on quit (`src/Snake.cpp:722`). Finally it missed three ambiguities that were
added by hand: Food naming both the category and one of its two kinds, Collision covering
eating as well as dying, and **Speed** meaning two opposite quantities — `getSpeed()`
returns a delay where larger is slower, while the display shows `200 - getSpeed()` where
larger is faster.

---

## 3. Smell delta — [6]

Reports: `lab2_3/audits/main.md` (21 findings at `main`, commit `3802598`) and
`lab2_3/audits/lab1-head.md` (28 findings at the Lab-1 head, commit `462c804` on
`feat/multiplayer` of the Lab-1 fork `BitecodesHub/Lab_1_Snake-Game`).

| | count | representative site (`file:line`) |
|---|---|---|
| Smells my Lab-1 PR **introduced** | 8 | `src/Snake.cpp:435` — every collision rule written twice, once per snake |
| Smells my Lab-1 PR **left untouched** | 20 | `src/Snake.cpp:165` — `GameBoard`, now 479 lines and 15 members |
| Smells my Lab-1 PR **removed** | 1 | `src/Snake.cpp:496` at `main` — an unread `isNewHighScore` local, displaced rather than deleted on purpose |

The removed row is not zero, but it is not a win either: nothing was cleaned up on
purpose. The dead local sat on a line the Lab-1 change had to rewrite for other reasons.

---

## 4. Rejected candidates — [6]

| smell reported | `file:line` | why it does not hold |
|---|---|---|
| Long Parameter List | `src/Snake.cpp:190` | `drawCell(x, y, c, color)` hits the four-parameter threshold with four irreducible arguments. A parameter object here would exist only to be unpacked on the next line. Contrast `Food::spawn`, where the fourth parameter is a repetition that grows with the number of players — that one was accepted |
| Data Class | `src/Snake.cpp:16` | `Position` is flagged for two public fields and no behaviour, but it carries `operator==` and is a value type. The behaviour a reviewer would push into it is the collision rules, which belong to the snake that owns the cells |
| Large Class | `src/Snake.cpp:557` | `Game` is 185 lines, which trips the line threshold, but it has 2 fields and 4 methods and 97 of those lines are one menu of literal box-drawing characters. It is a thin shell, not an over-stuffed class |
| Switch Statements | `src/Snake.cpp:332` | The catalog's fix is Replace Conditional with Polymorphism. This switch decodes hardware scan codes at the input boundary, where a switch is the right shape; a key-class hierarchy for four cases is more machinery than the problem |
| Refused Bequest | `src/Snake.cpp:62` | `SpecialFood` uses the whole parent interface — `spawn`, `getPosition`, `getSymbol` — and overrides exactly one method. That is plain overriding, not a refused inheritance |

---

## 5. Commit map — [7]

| # | sha | subject | what it is |
|---|---|---|---|
| 1 | c208fad | Add reviewed ubiquitous language glossary | glossary |
| 2 | 0566999 | Add code-smell audit of main | smell report |
| 3 | 5572084 | Write down how many snakes there are, in one place | **the refactor, alone** |
| 4 | 93c8676 | Add the second player | **the feature, alone** |

---

## 6. Two-run measurement — [4]

| | Run 1 (Lab 1) | Run 2 (commit 4) |
|---|---|---|
| Smells introduced | 8 | 2 |
| Lines changed, `git diff --shortstat -w` | +157 / −68 | +40 / −13 |
| Lines changed, **raw** (no `-w`) | +161 / −72 | +40 / −13 |
| Functions reached | 13, plus class-scope fields | 7, plus class-scope fields |
| Prompts to working code | 1 | 1 |
| Wall-clock time | not recorded in Lab 1 | one session, not timed separately |

Commit 3 (the refactor) on its own: **+103 / −45** `-w`, **+152 / −94** raw.

Both prompt counts are 1, and both need a caveat rather than a footnote-free number.
Run 1's single prompt is what the Lab-1 log records, and that log also records that
the assignment document was fetched and parsed, so the assistant already had the four
requirements before it read any code. Run 2's single prompt is one instruction covering
Parts A to F of this handout, issued to an agent that read the repository itself; no
later prompt in that session changed any code, and the four commits were reached without
a correction round. Neither number measures difficulty. Both measure how much of the
specification was handed over in one go, which is why the smells-introduced row is the
one worth comparing across groups.

Run 2's two introduced smells: a Temporary Field, `loser`, which is −1 for the whole game
and read once (`src/Snake.cpp:170`, read at `src/Snake.cpp:530`); and a residual hard-coded
player index in the key map, `snakes[1]` (`src/Snake.cpp:394`), which means a third player
still needs a third `switch` even though it no longer needs a third of anything else.

Run 2's seven functions: `Snake::checkCollisionWith` (new), `GameBoard::GameBoard`,
`GameBoard::drawTopStats`, `GameBoard::handleInput`, `GameBoard::endGame` (new),
`GameBoard::update`, `GameBoard::displayGameOver`. Run 1 reached those plus `Food::spawn`,
`Snake::Snake`, `Snake::checkSnakeCollision`, `GameBoard::~GameBoard`,
`GameBoard::saveHighScore`, `GameBoard::draw`, `GameBoard::getSpeedForDirection` and
`GameBoard::getScore`.

---

## 7. Analysis Q1–Q2 — [5]

**Q1.** Commit 3 fixed **Shotgun Surgery**, M3 in `audits/main.md` (`src/Snake.cpp:147`).
Before, "there is exactly one snake" was written nowhere and implied at 19 sites across
9 methods; adding a snake meant finding all 19. Lab 1 found them and raised them to 43.
Now it is one integer, `SNAKE_COUNT` (`src/Snake.cpp:160`), and everything else reads
`snakes.size()`. Adding a snake costs one edit plus a key map.

**Q2.** Same feature, same codebase: 157 changed lines became 40, 13 functions became 7,
8 introduced smells became 2. What did not change is the interesting part. Four functions
had to be touched in both runs — input, update, the top bar and the game-over screen —
because a second player genuinely needs a second key map, a second score label and a
loser announcement. The floor is real; run 1 was three times above it. One quirk survived
both runs: the tick delay still follows player 1's heading alone (`src/Snake.cpp:519`).

---

## 8. Analysis Q3–Q4 — [5]

**Q3.** No. The Lab-1 log records one prompt — *"Read the assignment requirements, gather
them into a table, make the required changes, and do not commit"* — and one attempt, which
went straight to `snake1`/`snake2`. Nothing in that prompt asks what should change first,
and the assignment's own scope rule ("if you think the code needs restructuring, that is a
finding — do not act on it") discourages it. Splitting the request in two is what would
have had to differ: ask what makes the change expensive, land that, then ask for the
feature. That is commits 3 and 4.

**Q4.** Partly. Measured: `SNAKE_COUNT` is 1, so every loop introduced runs exactly once,
and the new start position `w*(s+1)/(SNAKE_COUNT+1)` evaluates to `w/2`. A scratch harness
replayed both `spawn` implementations against one seed over 20,000 spawns with a body that
forces retries: identical placements, identical rand() draw count (79,120), so the random
stream is not shifted. Not covered: rendering, input, the game-over screen, the high-score
file. There are no tests, and none were added. To actually know, the board would need a
headless mode taking a fixed seed and a scripted key sequence and emitting a text trace per
tick, captured as a golden file before the refactor and re-run after.

---

## If you did not finish

Finished, with one thing stated plainly rather than hidden: the pull request has not been
opened yet. This clone's `origin` is the upstream repository rather than a fork, so branch
`lab2_3/design-first` is local until it is pushed to the fork
`BitecodesHub/Lab_1_Snake-Game` and a PR is opened against that fork's `main`.
