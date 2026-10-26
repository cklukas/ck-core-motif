---
nav_exclude: true
---
# MWM frame-part hit-test input validation

`IdentifyFramePart` returns `FRAME_NONE` when passed a null `ClientData`
pointer.  This is the existing “no frame part” sentinel, so malformed event
dispatch cannot dereference a missing client while valid hit testing remains
unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-part-hittest-inputs.py`.
