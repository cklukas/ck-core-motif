---
nav_exclude: true
---
# Mrm pointer-list append state

`UrmPlistAppendPointer` now rejects a null vector, non-positive slot count,
negative active count, or active count beyond the slot count before indexing
the list. This keeps partially initialized or inconsistent lists on a safe
failure path.
