---
nav_exclude: true
---
# WSM WM_WINDOWS reply allocation failure

WSM `WM_WINDOWS` reply unpacking now checks its window-entry array before
initializing entries or marking the reply owned. A failed allocation clears the
count and returns safely.

`tests/automated/check-wsm-win-reply-oom.py` protects this failure path.
