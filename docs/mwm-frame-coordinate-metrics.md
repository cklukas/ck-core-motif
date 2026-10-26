---
nav_exclude: true
---
# MWM frame-coordinate metric validation

`BaseWindowX`, `BaseWindowY`, `FrameX`, and `FrameY` return zero for a null
`ClientData` pointer.  They are integer coordinate helpers without an error
channel; zero is the safe no-frame result.  Valid coordinate calculations are
unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-coordinate-metrics.py`.
