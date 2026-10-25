---
nav_exclude: true
---
# Container outline count bounds

Container detail layout now rejects outline-segment counts that cannot fit in
the legacy `int` count before adding or allocating segments. The current
outline remains intact and the pass is marked incomplete rather than wrapping
the count into a small or negative allocation.

The focused source regression is
`tests/automated/check-container-outline-count-bounds.py`.
