---
nav_exclude: true
---
# Separator redisplay guards

`XmSeparator` computes its drawable rectangle by subtracting twice the
highlight thickness from the widget dimensions.  A small geometry transition
could underflow those unsigned dimensions, and an expose during realization or
resource replacement could pass an invalid window or GC to `XmeDrawSeparator`.

The expose handler now returns until the widget is realized, the separator GC
exists, the drawable rectangle is non-empty, and shadow GCs are present when a
shadow is requested.  Valid separators retain the existing orientation,
margin, separator type, and shadow rendering.
