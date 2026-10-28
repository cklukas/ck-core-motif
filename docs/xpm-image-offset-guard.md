---
nav_exclude: true
---

# XPM image offset guard

XPM image serialization now checks zero `cpp` and the maximum safe
`width * cpp + 1` value before computing the row offset, preventing wrapped
dimensions from reaching allocation and pixel-row logic.

Regression: `tests/automated/check-xpm-image-offset-guard.py`
