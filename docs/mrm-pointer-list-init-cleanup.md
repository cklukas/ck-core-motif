---
nav_exclude: true
---
# Mrm pointer-list initialization cleanup

If the pointer-vector allocation fails, `UrmPlistInit` now frees the list
structure it already allocated and clears the output pointer before returning
failure. Successful initialization and ownership rules are unchanged.
