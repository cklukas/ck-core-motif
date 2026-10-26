---
nav_exclude: true
---
# TextField destination context allocation

TextField destination-context setup now checks both its context record and
destination record allocations before initialization, callback registration,
or `XSaveContext()`.  A partial allocation is released and setup returns
`NULL` cleanly.

Regression coverage: `tests/automated/check-textfield-destination-context.py`.
