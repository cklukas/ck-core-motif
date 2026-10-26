---
nav_exclude: true
---
# MWM frame-part input validation

`GetFramePartInfo` returns `FALSE` when its `ClientData` input or any geometry
output pointer is null.  The guard runs before `TitleBarHeight` reads client
state and before the frame-part switch writes results.  Valid calls keep the
existing decoration and geometry calculations unchanged; callers can treat a
`FALSE` result as “no frame-part geometry was returned.”

The source-contract regression is
`tests/automated/check-mwm-frame-part-inputs.py`.
