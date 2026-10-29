---
nav_exclude: true
---
# TextOut visibility-edge bounds

`MakePositionVisible` compares the caret against the inner widget's right or
bottom edge before changing the scroll offset.  The edge is the allocation
minus a margin; converting that subtraction to `Position` first can wrap when
the margin is larger than a transient allocation.

The comparisons now widen the allocation and margin before subtraction and keep
the result signed.  Normal scrolling is unchanged, while tiny widgets use the
correct negative edge instead of an unrelated positive coordinate.  The
contract is protected by `tests/automated/check-textout-visible-edge-bounds.py`.
