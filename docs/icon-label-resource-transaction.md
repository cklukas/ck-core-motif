---
nav_exclude: true
---
# IconLabel resource transactions

`XmIconLabel` now skips text drawing when its render table is unavailable and
preserves the existing label and render table if a `SetValues` copy fails.
Preferred-size measurement and redisplay therefore share the same safe
fallback.

The focused source check is
`tests/automated/check-icon-label-geometry-bounds.py`.
