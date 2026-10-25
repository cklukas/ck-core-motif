---
nav_exclude: true
---
# PanedWindow managed-child allocation handling

PanedWindow now treats its managed-child list allocation as optional during
initialization and checks growth allocations in `ReManageChildren`.  Capacity
arithmetic is bounded, and a failed growth leaves the existing list intact and
stops rebuilding safely instead of indexing null or partially resized storage.

The regression check is `tests/automated/check-paned-managed-children-allocation.py`.
