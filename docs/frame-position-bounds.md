---
nav_exclude: true
---
# Frame child position bounds

Frame geometry computes child positions in `int` and then stores them in the
X11 `Position` type.  A wide frame or a large title/border combination can
produce a value outside the signed 16-bit range; a direct cast wraps the child
to the opposite side of the frame.

Child X positions now pass through a saturating helper before conversion.
Ordinary layouts are unchanged, while extreme transient geometry clamps to the
representable X11 range.  The contract is protected by
`tests/automated/check-frame-position-bounds.py`.
