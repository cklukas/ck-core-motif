---
nav_exclude: true
---

# FileSelectionBox push-button qualifier guard

The FileSelectionBox push-button path now checks its qualify-search procedure
before invoking it. If the procedure is absent, temporary mask and directory
strings are released and the action returns safely instead of dereferencing a
null callback slot or using an uninitialized qualified record.

The focused source contract is checked by
`tests/automated/check-filesb-pushbutton-qualify-guard.py`.
