---
nav_exclude: true
---
# Toolbar layout allocation failures

Toolbar layout uses temporary arrays for action selection and preferred
dimensions.  A failed allocation must not reach `choose_actions`, because that
routine writes all three arrays.  The layout now hides the existing action and
overflow presentation, reports every action as overflow, and returns cleanly
when scratch storage is unavailable.  The toolbar can be laid out again after
memory pressure is relieved.

The contract is protected by
`tests/automated/check-toolbar-layout-allocation.py`.

Changing `XmNtoolbarOverflowLabel` also updates the already-created overflow
button immediately; the resource no longer requires destroying and recreating
the toolbar to become visible.
