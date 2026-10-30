---
nav_exclude: true
---
# Container expand/collapse constraint guards

The Container expand/collapse action now returns safely when the focused child
has no Container constraint record.  Its outline state is private constraint
data; valid focus changes retain their existing expand/collapse behavior.

The source contract is checked by
`tests/automated/check-container-expand-constraint-guards.py`.
