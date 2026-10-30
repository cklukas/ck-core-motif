---
nav_exclude: true
---

# TabStack drag callback guards

TabStack’s drag callback now validates the callback widget, client TabStack,
and callback data before inspecting drag reasons or updating cursor icons.
Malformed callback invocations return safely; valid drag/drop cursor behavior
is unchanged.

The focused source-contract check is
`python3 tests/automated/check-tabstack-drag-callback-guards.py`.
