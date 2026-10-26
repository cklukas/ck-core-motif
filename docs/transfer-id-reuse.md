---
nav_exclude: true
---
# Transfer context reuse

Transfer contexts are now fully initialized before they are published on the
global context chain. Recycled contexts clear callback, widget, selection,
and drag/drop state after the old list is freed, preventing concurrent or
reused transfers from observing stale state.

Regression coverage: `tests/automated/check-transfer-id-reuse.py`.
