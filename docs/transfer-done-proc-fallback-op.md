---
nav_exclude: true
---

# Transfer completion callback operation

`CallDoneProcs` snapshots completion callbacks when possible.  Under
allocation pressure it falls back to fetching and invoking one callback at a
time so re-entrant callbacks remain safe.  The transfer operation is now
captured before that allocation attempt, ensuring the fallback receives the
same `XmTransferOp` instead of an uninitialized value.

Regression: `tests/automated/check-transfer-done-proc-fallback-op.py`
