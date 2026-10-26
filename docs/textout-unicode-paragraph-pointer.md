---
nav_exclude: true
---
# Unicode paragraph source-pointer validation

The Unicode paragraph cache now rejects a text-source read that reports
positive progress and bytes but supplies no source pointer. This prevents a
null pointer from reaching the paragraph snapshot `memcpy` path.

Regression coverage: `tests/automated/check-textout-unicode-paragraph-pointer.py`.
