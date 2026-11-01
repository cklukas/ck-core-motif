---
nav_exclude: true
---
# DataField highlight-list allocation

`XmDataField` now grows its internal highlight list transactionally. If the
replacement allocation fails or the requested record count cannot fit the Xt
allocator width, the existing list, count, and capacity remain valid. This
prevents selection or highlight updates from corrupting the field after a
memory-allocation failure.

Normal highlighting behavior, ordering, and public APIs are unchanged.

`tests/automated/check-datafield-highlight-allocation.py` protects this
failure-atomic growth contract.
