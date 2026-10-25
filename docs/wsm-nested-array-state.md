---
nav_exclude: true
---

# WSM nested array state

WSM request/reply packing now validates nested per-record arrays before size
calculation.  Positive `WSMWinInfo` data counts require a data list, and
positive WM-window entry counts require window/property lists.  Inconsistent
application-provided state is rejected without changing valid wire formats.
