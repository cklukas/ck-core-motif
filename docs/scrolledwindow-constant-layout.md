---
nav_exclude: true
---
# ScrolledWindow constant layout geometry

Constant-policy ScrolledWindow layout derives available work-area dimensions by
subtracting offsets, padding, and shadows from unsigned widget dimensions. A
collapsed scrolled window could wrap those values and configure an oversized
clip area or scrollbar region.

The layout now performs the available-size subtraction in signed arithmetic and
clamps the no-work-window area to zero. Normal scrolling geometry is unchanged
while transiently tiny windows remain bounded.
