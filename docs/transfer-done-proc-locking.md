---
nav_exclude: true
---
# Transfer done-procedure locking

Transfer-context done-procedure reclamation now occurs under the same process
lock used when registering callbacks. A finishing transfer can therefore not
free the callback list concurrently with `XmeTransferAddDoneProc`.

Regression coverage: `tests/automated/check-transfer-done-proc-locking.py`.
