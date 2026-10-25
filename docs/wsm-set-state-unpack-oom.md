---
nav_exclude: true
---
# WSM set-state unpack allocation failure

WSM `SET_STATE` request unpacking now checks its window-info count against
Xt's `Cardinal` allocation width before initializing entries. A failed or
unrepresentable allocation clears the count and returns before the request is
marked owned; successful requests mark the matching `set_state.allocated`
field for the existing cleanup path.

`tests/automated/check-wsm-set-state-unpack-oom.py` protects this behavior.
