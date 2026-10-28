---
nav_exclude: true
---

# UIL key-table scope

Argument validation uses one key-table pointer for both the support/duplicate
checks and the later value-type check. The redundant inner declaration was
removed so a future edit cannot accidentally update one lookup while using a
shadowed binding for the other.

Regression: `tests/automated/check-uil-keytable-scope.py`
