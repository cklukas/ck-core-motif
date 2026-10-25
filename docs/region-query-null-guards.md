---
nav_exclude: true
---
# XmRegion query and mutation null guards

The private `XmRegion` geometry helpers now handle absent inputs safely:
clearing and offsetting a null region are no-ops, an absent region is empty,
and equality treats two absent regions as equal. Point queries return false,
rectangle extents remain untouched when either input is absent, and the
rectangle count for an absent region is zero.

These rules preserve the existing behavior for valid regions while allowing
optional geometry state to be inspected or cleaned up without a crash. The
focused regression is `tests/automated/check-region-query-null-guards.py`.
