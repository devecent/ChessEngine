Run from the ChessEngine project root with PowerShell and g++ on PATH:

```powershell
$sources = Get-ChildItem src -Filter *.cpp | Where-Object Name -ne main.cpp | ForEach-Object FullName
$testExe = Join-Path $env:TEMP 'chessengine-regression.exe'
g++ -std=c++20 -O2 -Wall -Wextra -Werror -D_GLIBCXX_ASSERTIONS -I src tests/regression.cpp $sources -o $testExe
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed' }
& $testExe
if ($LASTEXITCODE -ne 0) { throw 'Regression checks failed' }
```

The suite covers perft from the starting position and Kiwipete, make/undo and
hash consistency, castling, promotion, en passant, repetition history, FEN clocks
and move suffixes, mate/stalemate at depth zero, quiescence captures, quiet
promotions, en passant, quiet check evasions, fifty-move draws, legal timeout fallbacks, board
restoration after interrupted searches, mate distances and bounds in the
transposition table, endgame king activity, capture ordering, killer slots,
quiet-cutoff history learning and bounds, fixed-depth root results, null-move
eligibility/cutoffs, en passant and timeout restoration after null moves, and UCI EOF.

Search currently treats a second occurrence as a draw to avoid repeating lines;
this is a search convention rather than a formal threefold-repetition claim.
The deadline check allows the current legal-move generation to finish.
