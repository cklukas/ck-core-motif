---
nav_exclude: true
---
# Transfer completion-list allocation width

`XmeTransferAddDoneProc` bounds its completion-list byte count to Xt's
`Cardinal` range before reallocating. The checked `size_t` value is now passed
with an explicit `Cardinal` conversion, documenting the allocator contract on
all supported pointer widths.

The focused regression is
`tests/automated/check-transfer-done-proc-allocation-width.py`.
