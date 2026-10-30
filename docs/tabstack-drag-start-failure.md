---
nav_exclude: true
---

# TabStack drag-start failure guard

TabStack drag setup now checks the result of `XmDragStart` before registering
drag callbacks. A failed drag creation returns cleanly instead of passing a
null drag widget to `XtAddCallback`; successful drag setup is unchanged.

The focused source-contract check is
`python3 tests/automated/check-tabstack-drag-start-failure.py`.
