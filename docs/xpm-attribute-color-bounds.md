---
nav_exclude: true
---

# XPM color-metadata bounds

`XpmCreateXpmImageFromImage` now validates caller-supplied color-table
metadata before scanning pixels.  When `XpmColorTable` or legacy `XpmInfos`
is requested, a non-empty table requires both the color and pixel arrays, and
an advertised transparent-color index must be inside `ncolors`.  Invalid
metadata returns `XpmFileInvalid` before any dereference while valid exported
color tables retain their existing behavior.

Regression: `tests/automated/check-xpm-attribute-color-bounds.py`
