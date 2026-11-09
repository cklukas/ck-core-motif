---
nav_exclude: true
---
# ToggleButtonGadget text-origin bounds

ToggleButtonGadget combines indicator, margin, highlight, shadow, and
rectangle dimensions to place text and accelerator rectangles. Those valid
inputs can exceed the signed legacy coordinate field when combined. Initial
layout and `ComputeSpace` now use wide arithmetic and clamp only at the final
LabelGadget `Position` storage boundary.

Indicator margin deltas and the automatically recomputed rectangle dimensions
are also saturated. The stored delta now records the actual bounded margin
increase, so later layout updates can remove exactly the space that was added.

The focused regression is
`tests/automated/check-togglebg-text-origin-bounds.py`.
