# Code smells at the Lab-1 branch head

Commit audited: `462c804` ("Add second player"), branch `feat/multiplayer` of the Lab-1
fork `BitecodesHub/Lab_1_Snake-Game`, the single commit of the Lab-1 pull request.
File audited: `src/Snake.cpp` (844 lines). Diff against `main`: +157 / −68 with `-w`,
+161 / −72 raw.

Same instrument as `main.md`: the `detect-code-smells` catalog for names and severities,
`review-accuracy-calibration` for the C1–C4 confidence levels, same prompt, report only.

## Findings

### Carried over from `main`, untouched

Same defect, new line number. Twenty of the twenty-one findings in `main.md` survive.

| # | Smell | Site | Sev | Conf | Note |
| --- | --- | --- | --- | --- | --- |
| M1 | Large Class | `src/Snake.cpp:165` | HIGH | C4 | `GameBoard` grew from 412 to 479 lines and from 13 to 15 data members |
| M2 | Divergent Change | `src/Snake.cpp:165` | HIGH | C3 | Unchanged; one more axis of change was added, namely which player lost |
| M3 | Shotgun Surgery | `src/Snake.cpp:170` | HIGH | C4 | Worse, not better. "Exactly one snake" became "exactly two snakes", named individually as `snake1` or `snake2` on 43 distinct lines: 38, 45, 52, 170, 171, 228, 229, 231–233, 238, 239, 279, 285, 298, 301, 311, 314, 328, 329, 375–378, 382–385, 425, 426, 428, 429, 443, 446, 451, 454, 473, 477, 490, 504, 557, 561 and 562. `main` had 19 |
| M4 | Long Method | `src/Snake.cpp:512` | HIGH | C4 | `displayGameOver` grew from 113 to 126 lines (512–637) |
| M5 | Long Method | `src/Snake.cpp:677` | MEDIUM | C4 | `showMenu`, 97 lines (677–773), untouched by Lab 1 |
| M6 | Long Method | `src/Snake.cpp:269` | MEDIUM | C3 | `draw` grew from 49 to 62 lines (269–330) |
| M7 | Long Method | `src/Snake.cpp:420` | HIGH | C4 | `update` grew from 39 to 73 lines (420–492) and from three collision rules to seven |
| M8 | Duplicate Code | `src/Snake.cpp:521` | MEDIUM | C4 | The box-rule loops are still there, now at lines 521, 530, 535, 601, 607, 620, 683, 744, 749, 766, 818 and 825 |
| M9 | Duplicate Code | `src/Snake.cpp:550` | LOW | C4 | The pad-to-column idiom is still there at lines 550, 563, 571, 579 and 594, with two hand-computed widths added at 549 and 562 |
| M10 | Duplicate Code | `src/Snake.cpp:212` | LOW | C4 | `setColor` still defined twice, at lines 212 and 651 |
| M11 | Duplicate Code | `src/Snake.cpp:291` | LOW | C4 | The food redraw call is still byte-identical in both branches of `draw`, at lines 291–292 and 318–319 |
| M13 | Dead Code | `src/Snake.cpp:639` | LOW | C4 | `getScore` (639), `getLevel` (640) and `getFoodEaten` (641) still have no call sites |
| M14 | Primitive Obsession | `src/Snake.cpp:83` | MEDIUM | C3 | Direction is still a raw `char`, decoded by hand at lines 88, 101–104, 121–126 and 506 |
| M15 | Primitive Obsession | `src/Snake.cpp:499` | LOW | C3 | The tick curve is still unnamed integers, at lines 499, 507 and 363 |
| M16 | Speculative Generality | `src/Snake.cpp:38` | LOW | C3 | `spawn` is still `virtual` and `SpecialFood` at line 69 still does not override it |
| M17 | Feature Envy | `src/Snake.cpp:38` | MEDIUM | C3 | Worse: `Food::spawn` now walks two snakes' bodies rather than one, at lines 45 and 52 |
| M18 | Inappropriate Intimacy | `src/Snake.cpp:279` | MEDIUM | C3 | `GameBoard` still indexes snake bodies (279–281, 285–287) and still mirrors their tails (178, 298, 301) |
| M19 | Temporary Field | `src/Snake.cpp:178` | MEDIUM | C3 | `oldTail1`, `oldTail2` (178), `oldFoodPos` (179) and `firstDraw` (180) |
| M20 | Message Chains | `src/Snake.cpp:291` | NIT | C2 | `currentFood->getPosition().x` at lines 291, 306, 318 and 321 |
| M21 | Comments | `src/Snake.cpp:22` | NIT | C2 | Lines 22, 38, 42, 50, 65 and 210. Two of `main`'s comments went the other way and were deleted from `update` |

### Introduced by the Lab-1 change

| # | Smell | Site | Sev | Conf | Evidence |
| --- | --- | --- | --- | --- | --- |
| I1 | Duplicate Code | `src/Snake.cpp:435` | HIGH | C4 | Every rule in `update` is written twice, once per snake: wall at 435 and 438, self at 443 and 446, snake-versus-snake at 451 and 454, eating at 472 and 476. Eight blocks where `main` had three |
| I2 | Duplicate Code | `src/Snake.cpp:279` | MEDIUM | C4 | Every drawing step is written twice: initial body at 279–282 and 285–288, tail erase at 298–300 and 301–303, head draw at 311–312 and 314–315, tail record at 328 and 329 |
| I3 | Duplicate Code | `src/Snake.cpp:45` | MEDIUM | C4 | `Food::spawn` now contains the same rejection scan twice, at 45–50 and 52–57, joined by a `continue` at 51 |
| I4 | Data Clumps | `src/Snake.cpp:168` | HIGH | C4 | Every per-player quantity became a numbered pair that must be extended in lockstep: `score1, score2` (168), `snake1, snake2` (170, 171), `oldTail1, oldTail2` (178) |
| I5 | Long Parameter List | `src/Snake.cpp:38` | MEDIUM | C3 | `spawn(int, int, const deque<Position>&, const deque<Position>&)` reaches four parameters by taking one body per snake, so a third snake changes the signature again |
| I6 | Temporary Field | `src/Snake.cpp:174` | MEDIUM | C3 | `loserPlayer` is an empty string for the whole game, written once at 461–465 and read once at 525 |
| I7 | Primitive Obsession | `src/Snake.cpp:461` | MEDIUM | C3 | Which player lost is held as display text. The rules layer builds the strings `"Player 1"`, `"Player 2"` and `"Player 1 & Player 2"` at 461–465 and the screen appends `" Lost!"` at 525 |
| I8 | Inappropriate Intimacy | `src/Snake.cpp:151` | MEDIUM | C3 | `Snake::checkSnakeCollision` walks another `Snake`'s body through `other.getBody()`, so each snake now depends on the internal representation of the others |

### Removed by the Lab-1 change

| # | Smell | Site at `main` | Sev | Conf | Note |
| --- | --- | --- | --- | --- | --- |
| M12 | Dead Code | `src/Snake.cpp:496` | LOW | C4 | The unread `isNewHighScore` local is gone. It was displaced, not deleted on purpose: the line it stood on became `int maxScore = max(score1, score2);` at `src/Snake.cpp:585`, which the next line actually reads |

## Totals

| | count |
| --- | --- |
| Carried over untouched | 20 |
| Introduced | 8 |
| Removed | 1 |
| Total at this commit | 28 |

## Candidates raised and rejected

The same five as at `main`, at their new sites, with the reasons in section 4 of
`lab2_3/REPORT.md`. R1 is worth re-reading here: the rule fired on `drawCell` at
`src/Snake.cpp:216` and was rejected, while the same rule fired on `Food::spawn` at
`src/Snake.cpp:38` and was accepted as I5. The parameter count is the same; what differs
is that one list is irreducible and the other grows with the number of players.

| # | Candidate | Site | Conf | Disposition |
| --- | --- | --- | --- | --- |
| R1 | Long Parameter List | `src/Snake.cpp:216` | C1 | Rejected — `drawCell(x, y, c, color)`, four irreducible arguments |
| R2 | Data Class | `src/Snake.cpp:16` | C1 | Rejected — `Position` is a value type carrying `operator==` |
| R3 | Large Class | `src/Snake.cpp:646` | C1 | Rejected — `Game` has 2 fields and 4 methods |
| R4 | Switch Statements | `src/Snake.cpp:374` | C2 | Rejected — scan-code decoding at the input boundary |
| R5 | Refused Bequest | `src/Snake.cpp:69` | C1 | Rejected — `SpecialFood` overrides one method and uses the rest |

## Note

No files were edited in the course of this audit. The Lab-1 fork was fetched read-only.
