---
nav_exclude: true
---
# ToggleSwitch graphics-context setup

`XmToggleSwitch` owns one mutable graphics context for its track, knob and label.
If allocation fails, expose skips drawing and destruction checks the handle
before freeing it. There is no partial set of drawing GCs to retain or release.

`tests/automated/run-toggle-switch-failures.py` injects a real `XCreateGC`
failure during construction and exercises expose and destruction. Pass `--build`
with the configured build directory and `--out` with a fresh absolute results
directory.
