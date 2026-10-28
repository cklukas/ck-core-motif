---
nav_exclude: true
---

# XPM parser entry validation

`xpmParseData` now rejects null parser-state or output-image arguments with
`XpmFileInvalid` before dereferencing them or starting header parsing. Parsed
zero-width or zero-height image headers are rejected before comment,
color-table, or pixel allocation work begins.

Regression: `tests/automated/check-xpm-parse-entry.py`
