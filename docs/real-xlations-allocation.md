---
nav_exclude: true
---
# Legacy real-translation allocation

The compatibility helper that expands virtual key bindings now checks its
initial translation buffer before writing the empty terminator.  Allocation
failure returns no translation safely; successful translation construction and
legacy key-binding behavior are unchanged.

`tests/automated/check-real-xlations-allocation.py` protects this contract.
