---
nav_exclude: true
---

# XPM compressed-reader wait status

The z-pipe reader now checks the result of `waitpid` after retrying interrupted
waits. A non-interrupted wait failure closes the pipe through the existing
failure path instead of interpreting an uninitialized child status.

Regression: `tests/automated/check-xpm-pipe-wait-status.py`
