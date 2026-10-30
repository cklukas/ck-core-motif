---
nav_exclude: true
---

# NumberBox graphics-context setup

NumberBox requires separate text, arrow, and invalid-state graphics contexts.
Initialization now rolls back every partially created GC when any allocation
fails, preventing leaked X resources and ensuring redisplay sees either a
complete drawing set or no drawing set.

The focused source contract is checked by
`tests/automated/check-number-box-gc-setup.py`.
