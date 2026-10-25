---
nav_exclude: true
---
# MainWindow auto-scroll rectangle guard

`GetVertRects` now initializes its output arguments to an empty set and checks
the two-rectangle allocation. If allocation fails, callers receive zero
rectangles and no invalid memory is written; successful geometry calculation is
unchanged.
