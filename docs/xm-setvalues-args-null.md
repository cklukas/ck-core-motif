---
nav_exclude: true
---
# Xm child update argument validation

`_XmSetValuesOnChildren` now returns before recursion when a nonzero argument
count is paired with a null `ArgList`. Empty updates (`num_args == 0`) and valid
argument lists retain their existing recursive behavior. This prevents malformed
partial-update calls from entering `XtSetValues` with an invalid array.
