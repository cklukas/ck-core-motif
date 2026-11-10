---
nav_exclude: true
---

# WSM display input validation

The common WSM display-metadata lookup rejects a null `Display` before it
interns protocol atoms.  The per-screen lookup also rejects negative screen
numbers and values outside `ScreenCount(display)` before it creates selection
atoms or cache state.  Dependent selection, screen, configuration, and target
lookup helpers therefore return their established null or `None` failure values
instead of passing invalid display or screen inputs into Xlib.

Regressions: `tests/automated/check-wsm-display-inputs.py` checks that the
guards occur before Xlib/cache operations; the native
`tests/automated/wsm-config-replacement.c` fixture exercises negative and
out-of-range screen values under Xvfb.
