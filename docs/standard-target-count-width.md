---
nav_exclude: true
---
# Standard target count width

`XmeStandardTargets` returns a validated non-negative `int` count for its
legacy API, while selection callbacks store lengths as `unsigned long`. The
standard conversion now widens that count explicitly before publication.

The focused regression is
`tests/automated/check-standard-target-count-width.py`.
