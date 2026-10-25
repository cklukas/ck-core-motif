---
nav_exclude: true
---

# Protocol null-slot handling

Protocol-manager teardown now skips null protocol slots. This keeps partially
initialized arrays from causing null dereferences while preserving normal
cleanup for valid protocol entries.

The focused regression check is
`tests/automated/check-protocol-null-slots.py`.
