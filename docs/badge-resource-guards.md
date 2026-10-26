---
nav_exclude: true
---
# Badge resource guards

`XmBadge` now preserves its render table when a `SetValues` copy fails and
skips the decorative frame when the widget is too small for a valid rectangle.
Text measurement and drawing continue to use the active render table.

The focused source check is `tests/automated/check-badge-text-metrics.py`.
