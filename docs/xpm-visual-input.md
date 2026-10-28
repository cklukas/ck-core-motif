---
nav_exclude: true
---

# XPM explicit visual input

`XpmCreateImageFromXpmImage` now rejects an `XpmVisual` attribute flag with a
null `visual` pointer before color setup reads visual fields.  Calls that omit
the flag continue to use the display's default visual, and valid explicit
visuals retain their existing behavior.

Regression: `tests/automated/check-xpm-visual-input.py`
