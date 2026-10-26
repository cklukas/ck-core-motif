---
nav_exclude: true
---
# MWM title-text rectangle input validation

`GetTextBox` now returns without writing when passed a null `ClientData` or
`XRectangle` pointer.  This guard precedes title-area lookup and its fallback
rectangle writes; valid title-text geometry is unchanged.

The source-contract regression is
`tests/automated/check-mwm-text-box-inputs.py`.
