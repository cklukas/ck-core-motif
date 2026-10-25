---
nav_exclude: true
---
# WSM WM_WINDOWS reply allocation failure

WSM `WM_WINDOWS` reply unpacking now checks its entry count against Xt's
`Cardinal` allocation width before initializing entries or marking the reply
owned. A failed or unrepresentable allocation clears the count and returns
safely.

`tests/automated/check-wsm-win-reply-oom.py` protects this failure path.
