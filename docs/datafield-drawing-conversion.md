---
nav_exclude: true
---

# DataField drawing conversion

Wide-character DataField rendering now preserves `wcstombs` results as
`size_t` and rejects conversion failure or lengths that cannot be passed to
the legacy Xft/Xlib drawing APIs. Invalid text is skipped instead of being
drawn with a wrapped length.

Regression coverage:

```text
python3 tests/automated/check-datafield-drawing-conversion.py
```
