---
nav_exclude: true
---

# ProgressBar graphics-context setup

ProgressBar requires its track and fill graphics contexts as a pair. Its
initialization now rolls back every partially created GC when either required
allocation fails, preventing leaked X resources and leaving redisplay in a
consistent no-drawing state. The optional text GC remains an independent
fallback: text rendering can use the track GC when it is unavailable.

The focused source contract is checked by
`tests/automated/check-progress-bar-gc-setup.py`.
