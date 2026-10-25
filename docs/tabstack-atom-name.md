---
nav_exclude: true
---
# TabStack atom-name portability

TabStack derives its private X atom name from the process ID.  The old code
cast `getpid()` to `int` and used an unbounded format write, which could lose
high bits on platforms with a wider `pid_t`.

The name now formats a `long` process ID into the existing 64-byte buffer with
`snprintf`.  The `ICS_TAB_PID_<pid>` naming contract remains unchanged for
normal process IDs.

`tests/automated/check-tabstack-atom-name.py` protects this behavior.
