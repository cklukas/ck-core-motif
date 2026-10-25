---
nav_exclude: true
---

# Trait hash portability

Trait-key hashing now mixes pointer and quark bits through `uintptr_t` before
converting to Motif's existing `XmHashValue` type. This avoids signed pointer
arithmetic overflow and retains useful address bits on 64-bit hosts without
changing trait lookup or registration APIs.

The focused regression check is
`tests/automated/check-trait-hash-portability.py`.
