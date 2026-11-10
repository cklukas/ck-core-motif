---
nav_exclude: true
---

# WSM diagnostic input validation

The WSM request and reply formatting helpers now accept a missing input record
and return a stable `<none>` diagnostic.  Diagnostic reporting therefore does
not turn a prior protocol or caller failure into a null-pointer crash.

Regression: the native `tests/automated/wsm-config-replacement.c` fixture
checks both diagnostic helpers before it opens its Xvfb display.
