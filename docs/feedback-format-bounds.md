---
nav_exclude: true
---
# Mwm feedback text bounds

The move/resize feedback overlay formats coordinates and dimensions into
internal text buffers. Those buffers now have room for full-width signed
coordinates and unsigned dimensions, and both updates use `snprintf`. The
size path also uses unsigned conversion specifiers matching its API types, so
large 32-bit dimensions are represented correctly without overwriting adjacent
state.

`tests/automated/check-feedback-format-bounds.py` guards the buffer and format
contract.
