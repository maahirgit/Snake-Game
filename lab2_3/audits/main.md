# Code smells at `main` — the code as received

Commit audited: `3802598` ("Final Readme file"), branch `main`.
File audited: `src/Snake.cpp` (755 lines, one translation unit). `README.md` read for
context only.

Produced with the `detect-code-smells` skill, severities from its catalog, confidence
levels from `review-accuracy-calibration`.

## Prompt used

> Find the code smells in this repo. Report only — do not edit anything. Every smell must
> cite `file:line`; if you cannot point at a line it is not a finding. Assign each one a
> confidence level from the calibration skill (C1–C4) and a severity.

## Confidence key

| Level | Meaning | Reported? |
| --- | --- | --- |
| C4 | Direct evidence — counted lines, counted call sites, provably unused symbol | yes |
| C3 | Strong reasoning from an established pattern | yes |
| C2 | Plausible but depends on context not visible here | yes, at NIT |
| C1 | Speculative — investigated, then dropped | no, listed under "rejected" |

## Findings

| # | Smell | Site | Sev | Conf | Evidence |
| --- | --- | --- | --- | --- | --- |
| M1 | Large Class | `src/Snake.cpp:142` | HIGH | C4 | `GameBoard` spans lines 142–553: 412 lines, 13 data members, 18 methods, covering rendering, input, timing, rules, scoring, file persistence and the game-over screen |
| M2 | Divergent Change | `src/Snake.cpp:142` | HIGH | C3 | The same class must be edited to change the console colour scheme, the level curve, the key bindings, or the collision rules — four unrelated axes of change |
| M3 | Shotgun Surgery | `src/Snake.cpp:147` | HIGH | C4 | "There is exactly one snake" is never written down. The single `Snake*` member is re-asserted independently at lines 202, 204, 205, 250, 263, 273, 287, 310, 333–336, 340–343, 383, 386, 394, 401, 414, 428, 473 and 474 — 19 sites in 9 methods, each of which must be found and edited to add a second snake |
| M4 | Long Method | `src/Snake.cpp:436` | HIGH | C4 | `displayGameOver` is 113 lines (436–548) and does five things: persist the high score, clear the screen, draw three separate boxes, and block on a keypress |
| M5 | Long Method | `src/Snake.cpp:588` | MEDIUM | C4 | `showMenu` is 97 lines (588–684), of which about 80 are literal box-drawing characters |
| M6 | Long Method | `src/Snake.cpp:240` | MEDIUM | C3 | `draw` is 49 lines (240–288) with two full-body branches selected by a flag field |
| M7 | Long Method | `src/Snake.cpp:378` | MEDIUM | C3 | `update` is 39 lines (378–416) and mixes movement, three collision rules, scoring, levelling and food respawn |
| M8 | Duplicate Code | `src/Snake.cpp:445` | MEDIUM | C4 | `for(int i=0;i<50;i++) cout << (char)205;` and its siblings appear 12 times: lines 445, 451, 456, 512, 518, 531, 594, 655, 660, 677, 729, 736 |
| M9 | Duplicate Code | `src/Snake.cpp:466` | LOW | C4 | The pad-to-column idiom `for(int i = to_string(x).length(); i < N; i++) cout << " ";` appears 5 times with a different magic width each time: lines 466, 474, 482, 490, 505 |
| M10 | Duplicate Code | `src/Snake.cpp:186` | LOW | C4 | `setColor` is defined twice with identical bodies, once in `GameBoard` at line 186 and once in `Game` at line 562 |
| M11 | Duplicate Code | `src/Snake.cpp:256` | LOW | C4 | The two-line food redraw call is byte-identical in both branches of `draw`: lines 256–257 and 277–278 |
| M12 | Dead Code | `src/Snake.cpp:496` | LOW | C4 | `bool isNewHighScore = (score > highScore - score);` is assigned and never read. The expression is also meaningless — the branch below at line 497 tests something else entirely |
| M13 | Dead Code | `src/Snake.cpp:550` | LOW | C4 | `getScore` (550), `getLevel` (551) and `getFoodEaten` (552) have no call sites anywhere in the translation unit, which is the whole program |
| M14 | Primitive Obsession | `src/Snake.cpp:76` | MEDIUM | C3 | Direction is a raw `char` with four magic values, decoded by hand at lines 88–91, 108–113 and 430 |
| M15 | Primitive Obsession | `src/Snake.cpp:423` | LOW | C3 | The tick curve is four unnamed integers in one expression, `max(50, 150 - (level - 1) * 10)`, with a fifth at line 431 and a sixth at line 321 |
| M16 | Speculative Generality | `src/Snake.cpp:38` | LOW | C3 | `spawn` is declared `virtual` with a comment saying a child will override it; the only child, `SpecialFood` at line 62, does not |
| M17 | Feature Envy | `src/Snake.cpp:38` | MEDIUM | C3 | `Food::spawn` is handed the snake's `deque<Position>` and iterates its contents at lines 45–50 to decide placement, using the snake's data more than its own |
| M18 | Inappropriate Intimacy | `src/Snake.cpp:250` | MEDIUM | C3 | `GameBoard` indexes into the snake's body deque (250–252), and keeps its own copy of the snake's tail in `oldTail` (153) so it can erase the cell the snake is about to leave (263) |
| M19 | Temporary Field | `src/Snake.cpp:153` | MEDIUM | C3 | `oldTail` (153), `oldFoodPos` (154) and `firstDraw` (155) are meaningless outside the incremental-redraw path in `draw`, and `firstDraw` is true for exactly one call |
| M20 | Message Chains | `src/Snake.cpp:256` | NIT | C2 | `currentFood->getPosition().x` chains twice, repeated at lines 256, 269, 277 and 280 |
| M21 | Comments | `src/Snake.cpp:22` | NIT | C2 | Comments restate the line above rather than explain it: lines 22, 38, 42, 50, 58, 94, 184 |

**21 findings.** 3 HIGH, 7 MEDIUM, 7 LOW, 2 NIT, plus 2 counted under MEDIUM above.

## Candidates raised and rejected

Reported by the catalog's rules, investigated, and dropped. Reasons are in section 4 of
`lab2_3/REPORT.md`.

| # | Candidate | Site | Conf | Disposition |
| --- | --- | --- | --- | --- |
| R1 | Long Parameter List | `src/Snake.cpp:190` | C1 | Rejected — `drawCell(x, y, c, color)` hits the four-parameter threshold with four irreducible arguments |
| R2 | Data Class | `src/Snake.cpp:16` | C1 | Rejected — `Position` is a value type and does carry `operator==` |
| R3 | Large Class | `src/Snake.cpp:557` | C1 | Rejected — `Game` is 185 lines but has 2 fields and 4 methods, and 97 of those lines are one menu |
| R4 | Switch Statements | `src/Snake.cpp:332` | C2 | Rejected — decoding scan codes at the input boundary is what a switch is for |
| R5 | Refused Bequest | `src/Snake.cpp:62` | C1 | Rejected — `SpecialFood` uses the whole parent interface and overrides one method |

## Note

No files were edited in the course of this audit.
