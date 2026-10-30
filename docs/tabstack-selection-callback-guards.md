---
nav_exclude: true
---

# TabStack selection callback guards

TabStack’s selection callback now validates the callback widget/data and parent
TabStack before reading child state. It also stops safely when no active child
exists to replace. Valid tab selection continues through the existing active
child swap and redraw path.

The focused source-contract check is
`python3 tests/automated/check-tabstack-selection-callback-guards.py`.
