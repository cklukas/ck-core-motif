---
nav_exclude: true
---
# Transfer completion callbacks

Transfer completion callbacks are invoked during cancellation, replacement,
and normal destruction. `XmeTransferAddDoneProc` now rejects a null callback
before it can enter the completion list, so teardown cannot dereference an
invalid function pointer.

The focused regression is
`tests/automated/check-transfer-done-proc-null.py`.
