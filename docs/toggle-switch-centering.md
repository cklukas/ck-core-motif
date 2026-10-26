---
nav_exclude: true
---
# Toggle-switch knob centering

The toggle knob is vertically centered from the track and knob dimensions.
This keeps the control symmetric when its geometry changes and removes the
previous hard-coded offset that placed the 18-pixel knob one pixel too low in
the 24-pixel track.

The focused source contract is checked by
`tests/automated/check-toggle-switch-centering.py`.
