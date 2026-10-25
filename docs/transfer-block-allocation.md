---
nav_exclude: true
---
# Transfer request-block allocation

`XmTransferValue` adds each requested target to a linked transfer-block list.
The block allocator now reports `NULL` on failure, and the caller unlocks and
returns before mutating request counts or dereferencing the block.

Successful requests retain their ordering, counters, and selection behavior;
failed allocation leaves the transfer context unchanged.
