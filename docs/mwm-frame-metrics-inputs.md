---
nav_exclude: true
---
# MWM frame-metric input validation

`FrameWidth` and `FrameHeight` return zero for a null `ClientData` pointer.
These unsigned metric helpers have no separate error channel, and zero is the
safe “no frame” value.  Existing valid frame calculations are unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-metrics-inputs.py`.
