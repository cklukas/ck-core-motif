---
nav_exclude: true
---
# Badge resource and drawing safety

`XmBadge` now skips redisplay until its window and foreground GC are
available. Label replacement releases the previous owned `XmString` only
after a successful copy, retaining the old label if allocation fails.
Successful drawing and resource behavior are unchanged.

`tests/automated/check-badge-resource-safety.py` protects these paths.
