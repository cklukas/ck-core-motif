---
nav_exclude: true
---
# Mrm argument-list descriptor bounds

Mrm argument-list initialization now keeps descriptor sizes in `size_t` and
rejects counts whose inline descriptor cannot fit the format's 16-bit
`MrmSize` field. The existing zero-count rejection remains explicit, preventing
subtractive underflow before record allocation.

The source-level regression is `tests/automated/check-mrm-arg-descriptor-bounds.py`.
