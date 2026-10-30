---
nav_exclude: true
---
# Transfer completion callbacks

Transfer completion callbacks are invoked during cancellation, replacement,
and normal destruction. `XmeTransferAddDoneProc` now rejects a null callback
before it can enter the completion list, and rejects registration after the
transfer is flushed because that callback can no longer be delivered. Teardown
therefore does not retain unreachable callback allocations or dereference an
invalid function pointer.

The focused regression is
`tests/automated/check-transfer-done-proc-null.py`.

The flushed-state regression is
`tests/automated/check-transfer-done-proc-flushed.py`.
