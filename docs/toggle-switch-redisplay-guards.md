---
nav_exclude: true
---
# ToggleSwitch redisplay guards

The switch skips drawing when its window or owned GC is absent. Content clips
to the allocated interior; one-pixel allocations do not wrap unsigned geometry.
Focus borders are omitted when the allocation cannot contain their thickness.

`tests/automated/run-toggle-switch-pixels.py` exercises missing-GC redisplay and
one-pixel allocations in addition to actual pixel symmetry, RTL, and disabled
rendering. Pass `--build` with the configured build directory and `--out` with
a fresh absolute results directory.
