---
nav_exclude: true
---
# Divider resource and drawing safety

`XmDivider` now skips redisplay until its window and foreground GC are
available. Label replacement releases the previous owned `XmString` only
after a successful copy, retaining the old label if allocation fails.
Successful divider rendering and resource behavior are unchanged.

`tests/automated/check-divider-resource-safety.py` protects these paths.
