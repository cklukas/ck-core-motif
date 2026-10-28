---
nav_exclude: true
---

# XPM returned-comment status

When comment-return mode is enabled, all three comment extraction calls now
propagate allocation failures through the parser cleanup path.  The parser no
longer reports success with silently missing comment data after `XpmNoMemory`.

Regression: `tests/automated/check-xpm-comment-status.py`
