# Hash-table record allocation

`_XmAllocHashTable` now checks the opaque table-record allocation before
initializing its callbacks or bucket array. Allocation failure returns `NULL`
without dereferencing an incomplete table. The existing bucket-allocation
rollback remains in force for failures after the record has been created.

The focused regression is `tests/automated/check-hash-table-allocation.py`.
