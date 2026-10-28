---
nav_exclude: true
---

# Clipboard target-count narrowing

`XmClipboardInquireCount` now divides the returned target-byte length into an
atom count before converting to the public `int` result. The existing
`INT_MAX` check proves that conversion is safe and keeps strict builds from
reporting a misleading signed-conversion warning.

Regression: `tests/automated/check-clipboard-count-narrowing.py`
