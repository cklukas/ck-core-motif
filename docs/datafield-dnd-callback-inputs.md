---
nav_exclude: true
---
# DataField DnD callback inputs

The legacy DataField drag-motion and drop callbacks now validate both the
widget and callback payload before reading drag context, drop action, or
coordinates. Malformed or teardown-time DnD notifications therefore return
safely.

Regression coverage: `tests/automated/check-datafield-dnd-callback-inputs.py`.
