---
nav_exclude: true
---
# MWM frame-regeneration input validation

`RegenerateClientFrame` returns without changing windows or geometry when its
client record is null.  The guard precedes decoration lookup; valid frame
recalculation and subwindow resizing are unchanged.

The source-contract regression is
`tests/automated/check-mwm-frame-regeneration-inputs.py`.
