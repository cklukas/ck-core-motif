---
nav_exclude: true
---

# Tree centering arithmetic

`XmTree` centers each node and its disclosure button inside the computed
bounding box.  The box, child, and disclosure dimensions are Xt `Dimension`
values, so subtracting them directly can wrap when a child is larger than its
available space.  The centering differences now use signed `int` arithmetic
in both horizontal and vertical tree orientations; oversized children remain
near the box instead of receiving a wrapped coordinate.

Compressed-child width accumulation uses the same signed conversion when
subtracting the horizontal compression delta, keeping parent layout widths
valid when the delta exceeds a child’s width.

The source contract is checked by
`tests/automated/check-tree-centering-arithmetic.py`.
