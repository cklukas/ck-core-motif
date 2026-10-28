---
nav_exclude: true
---
# Calendar month bounds

Calendar’s month-length and month-name helpers now validate the one-based
month before indexing their fixed tables. Invalid months return no days or an
empty name, while valid leap-year and localized calendar rendering behavior is
unchanged.

The source contract is checked by
`tests/automated/check-calendar-month-bounds.py`.
