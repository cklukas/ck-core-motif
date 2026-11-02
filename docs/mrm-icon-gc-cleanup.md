---
nav_exclude: true
---

# Mrm icon graphics-context cleanup

All four icon-mapping implementations create an `XImage` and a server pixmap
before acquiring the graphics context used for `XPutImage`. If graphics context
creation fails, they now release both partial resources and reset the output
pixmap to zero before reporting failure. Successful icon mapping is unchanged.

The focused regression is `tests/automated/check-mrm-icon-gc-cleanup.py`.
