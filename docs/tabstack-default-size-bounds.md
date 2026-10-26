---
nav_exclude: true
---
# TabStack default-size bounds

When no explicit geometry is supplied, `XmTabStack` derives each dimension
from its shadow thickness, margin, and a fixed tab-area allowance.  The
calculation now uses widened arithmetic and clamps to the X11 `Dimension`
range before assignment, preventing large resource values from wrapping to a
small default widget.

The focused source regression is
`tests/automated/check-tabstack-default-size-bounds.py`.
