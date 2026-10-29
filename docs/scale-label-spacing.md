---
nav_exclude: true
---

# Scale label-spacing arithmetic

When `XmScale` derives a fallback size from several managed labels, it adds
space for the tick/slider allowance only when that allowance exceeds half the
largest label extent.  The allowance and extent are Motif `Dimension` values,
which are unsigned.  The subtraction now uses signed `int` intermediates in
both orientations; a label that is taller or wider than the allowance no
longer wraps to a huge positive gap and inflate the requested Scale geometry.

The source contract is checked by
`tests/automated/check-scale-label-spacing.py`.
