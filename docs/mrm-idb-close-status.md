---
nav_exclude: true
---
# Mrm IDB close status and cleanup

Low-level IDB close now frees its state when the operating-system close fails,
and reports unlink failures when deleting a file. The public `UrmIdbCloseFile`
entry point now returns the low-level close result instead of always reporting
success.

The source-level regression is `tests/automated/check-mrm-idb-close-status.py`.
