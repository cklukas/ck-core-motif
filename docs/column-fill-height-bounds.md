---
nav_exclude: true
---

# Column flush-fill height bounds

`XmColumn` gives flush-fill children the column’s inner height after subtracting
shadow and margin space.  A very small allocation can make that signed
calculation negative; converting it directly to Xt’s unsigned `Dimension`
would otherwise create an enormous child.  The layout now clamps the inner
height to one pixel before the conversion, preserving a valid geometry request
while retaining ordinary sizing behavior.

The source contract is checked by
`tests/automated/check-column-fill-height-bounds.py`.
