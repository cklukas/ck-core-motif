---
nav_exclude: true
---

# XPM parser entry validation

`xpmParseData` now rejects null parser-state or output-image arguments with
`XpmFileInvalid` before dereferencing them or starting header parsing.

Regression: `tests/automated/check-xpm-parse-entry.py`
