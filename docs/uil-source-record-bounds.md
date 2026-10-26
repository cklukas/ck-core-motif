---
nav_exclude: true
---
# UIL source-record allocation bounds

UIL diagnostic and machine-code records use flexible trailing text/data. Their
lengths now stay in `size_t`, reject negative machine-code lengths, and check
the complete struct-plus-payload size before calling the allocator. Allocation
failure also returns without publishing a partially initialized record.

The source contract is checked by:

```sh
python3 tests/automated/check-uil-source-record-bounds.py
```
