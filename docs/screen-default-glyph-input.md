---
nav_exclude: true
---

# Default-glyph lookup input validation

`_XmGetDefaultGlyphPixmap` now returns `None` and clears requested dimensions
when its screen is missing or unresolved. Valid glyph pixmap and dimension
lookups remain unchanged.

The focused regression check is
`tests/automated/check-screen-default-glyph-input.py`.
