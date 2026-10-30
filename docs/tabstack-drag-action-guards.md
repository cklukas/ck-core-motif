---
nav_exclude: true
---

# TabStack drag action guards

`XmTabBoxDragTab` now validates its invoking widget, button event, parent
chain, and TabStack before accessing drag state. It also resolves the dragged
tab child on every build configuration, preventing an uninitialized client
pointer when `TEAR_OFF_TABS` is disabled. Tear-off-only eligibility checks
remain conditional; valid drag setup is otherwise unchanged.

The focused source-contract check is
`python3 tests/automated/check-tabstack-drag-action-guards.py`.
