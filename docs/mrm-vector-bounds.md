---
nav_exclude: true
---
# Mrm vector-copy bounds

Mrm vector value copying rejects negative counts and byte sizes before creating
CString or Char8 vector copies, and checks the vector-count multiplication
against `size_t`. Invalid vector metadata leaves the source value untouched;
valid UID vectors retain their existing copy behavior.

The source-level regression is `tests/automated/check-mrm-vector-bounds.py`.
