---
nav_exclude: true
---
# Transfer completion and application locking

When a failed drop has no queued requests, `XmTransferDone` must finalize the
transfer immediately. Finalization invokes completion callbacks, which may
legitimately call back into Motif. The application lock is now released before
that finalization, avoiding a reentrant deadlock while retaining the existing
drop failure status updates.

Regression coverage: `tests/automated/check-transfer-done-unlocks-before-finish.py`.
