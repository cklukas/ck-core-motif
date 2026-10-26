---
nav_exclude: true
---

# DataField SetValues selection-array replacement

When a new selection-scan array is supplied through `SetValues`, DataField now
allocates the dedicated replacement before releasing the old widget array. A
failed allocation leaves the old array and the new resource pointer valid,
avoiding a null `memcpy` and preventing loss of the existing widget state.

The focused source contract is checked by
`tests/automated/check-datafield-setvalues-selection-array.py`.
