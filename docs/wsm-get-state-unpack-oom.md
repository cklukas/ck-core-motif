---
nav_exclude: true
---
# WSM get-state unpack allocation failure

WSM `GET_STATE` reply unpacking now checks its window-info count against Xt's
`Cardinal` allocation width before initializing entries or marking the reply
owned. A failed or unrepresentable allocation clears the count and returns, so
reply dispatch and cleanup remain safe.

`tests/automated/check-wsm-get-state-unpack-oom.py` protects this behavior.
