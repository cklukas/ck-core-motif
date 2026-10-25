---
nav_exclude: true
---
# ToggleButtonGadget box bounds

`DrawBox` now skips shadow and fill drawing when its computed edge is
non-positive or either shadow GC is unavailable.  This prevents invalid X draw
requests for collapsed gadget geometry while preserving the existing fill
margin behavior for valid boxes.

The focused regression is `tests/automated/check-togglebg-box-bounds.py`.
