---
nav_exclude: true
---
# Position-based list deletion input validation

Position-based deletion now rejects null widgets before application-context
access. The shared position helper also rejects null arrays and non-positive
counts, avoiding invalid iteration while preserving valid deletion semantics.

`tests/automated/check-list-position-delete-inputs.py` protects this contract.
