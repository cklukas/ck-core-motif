---
nav_exclude: true
---
# Tab-list proposal count bounds

`XmStringTableProposeTablist` now uses an unsigned index matching the internal
tab-list count and rejects exhaustion before adding a tab or incrementing the
per-line index.  This prevents counter wraparound while preserving ordinary
tab-list measurement.

`tests/automated/check-tablist-count-bounds.py` protects both guards.
