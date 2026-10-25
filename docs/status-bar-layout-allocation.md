---
nav_exclude: true
---

# Status-bar layout allocation failure

`XmStatusBar` uses a temporary priority array while selecting the items that
fit in the current width.  If that allocation fails, the widget now hides its
presentation children, reports every item as overflow, clears its layout
reentrancy flag, and returns cleanly.  Applications can use
`XmStatusBarGetOverflowCount` to retain or expose their own recovery UI.

The normal priority and overflow behavior is unchanged.  The source-level
regression is `tests/automated/check-status-bar-layout-allocation.py`.
