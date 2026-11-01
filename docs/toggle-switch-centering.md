---
nav_exclude: true
---
# Toggle-switch knob centering

The toggle knob is vertically centered from the track and knob dimensions.
This keeps the control symmetric when its geometry changes and removes the
previous hard-coded offset that placed the 18-pixel knob one pixel too low in
the 24-pixel track.

The native pixel contract is checked by
`tests/automated/run-toggle-switch-pixels.py`.

The native runner requires `--build` pointing to the configured build directory
and `--out` pointing to a fresh absolute results directory. It executes the real
widget rather than checking implementation text.
