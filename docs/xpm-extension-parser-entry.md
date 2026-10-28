---
nav_exclude: true
---

# XPM extension-parser entry validation

`xpmParseExtensions` now rejects null parser state and output pointers with
`XpmFileInvalid` before scanning or allocating extension records.

Regression: `tests/automated/check-xpm-extension-parser-entry.py`
