---
nav_exclude: true
---
# Text primary-selection reference counts

Text and TextField primary-selection completion paths now treat a non-positive
reference count as terminal before decrementing it. Late or duplicate
completion callbacks therefore cannot underflow the shared selection state.

Regression coverage: `tests/automated/check-text-primary-selection-refcount.py`.
