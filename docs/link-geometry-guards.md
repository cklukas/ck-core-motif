---
nav_exclude: true
---
# Link geometry and resource guards

`XmLink` now clamps underline rules to the widget width, skips focus frames
that cannot fit, and keeps the existing render table when a `SetValues` copy
fails. The active render-table metrics remain the source for label geometry.

The focused source check is `tests/automated/check-link-text-metrics.py`.
