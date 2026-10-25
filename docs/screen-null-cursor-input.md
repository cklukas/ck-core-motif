---
nav_exclude: true
---

# Null-cursor input validation

`XmeGetNullCursor` now returns `None` for a missing widget or unresolved
screen, before accessing Xt or screen state. Normal cursor creation and cache
reuse remain unchanged.

The focused regression check is
`tests/automated/check-screen-null-cursor-input.py`.
