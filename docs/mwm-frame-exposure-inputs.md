---
nav_exclude: true
---
# MWM frame exposure input validation

`FrameWindow` returns `FALSE` for a null client record.  `FrameExposureProc`
and `BaseWinExposureProc` return without drawing for the same input.  These
guards run before window, appearance, or display-list access; valid framing and
exposure behavior is unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-exposure-inputs.py`.
