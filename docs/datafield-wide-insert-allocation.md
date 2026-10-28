---
nav_exclude: true
---

# DataField wide insertion allocation

The DataField modify-verify callback now validates the wide-character count
before evaluating `(insert_length + 1) * sizeof(wchar_t)`. Counts that cannot
fit the allocator's `Cardinal` size are rejected without wrapping the addition
or creating an undersized buffer.

Regression check:

```text
python3 tests/automated/check-datafield-wide-insert-allocation.py
```
