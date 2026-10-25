---
nav_exclude: true
---
# XmString context utility null guards

Context reinitialization, copying, and cleanup now tolerate null or identical
context pointers without assertion termination. Valid context behavior is
unchanged; malformed cleanup paths become harmless no-ops.

The regression check is
`tests/automated/check-xmstring-context-null-utils.py`.
