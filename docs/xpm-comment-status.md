---
nav_exclude: true
---

# XPM returned-comment status

When comment-return mode is enabled, all three comment extraction calls in
both the parser and image-create paths now propagate allocation failures
through their cleanup paths.  Neither path reports success with silently
missing comment data after `XpmNoMemory`.

Regression: `tests/automated/check-xpm-comment-status.py`
