---
nav_exclude: true
---

# Notebook page and scroller coordinates

`XmNotebook` positions the page scroller relative to the active page.  The
scroller may be wider than a temporarily narrow page, so the position
calculation can legitimately be negative.  Layout coordinates now use signed
`int` values instead of `Dimension`; this prevents unsigned subtraction from
wrapping a negative position into a far-away coordinate.

The source contract is checked by
`tests/automated/check-notebook-page-coordinates.py`.
