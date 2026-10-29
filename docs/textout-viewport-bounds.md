---
nav_exclude: true
---
# TextOut viewport bounds

TextOut recalculates horizontal and vertical offsets after measuring a line.
During a transient resize, highlight, shadow, and text margins can consume the
entire widget allocation.  Treating the remaining viewport as a negative
dimension makes the offset calculation move in the wrong direction and can
leave a tiny text widget scrolled away from its content.

The offset paths now widen the allocation and margin arithmetic and clamp a
margin-only viewport to zero.  Normal viewports keep the same dimensions and
scroll behavior.  The contract is protected by
`tests/automated/check-textout-viewport-bounds.py`.
