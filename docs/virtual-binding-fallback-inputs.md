---
nav_exclude: true
---
# Virtual-binding fallback loader inputs

The fallback virtual-key binding loader now rejects a null display or output
pointer before clearing the output or interning X11 atoms. Valid fallback
lookup, allocation, and property-publication behavior is unchanged.

The focused regression check is
`tests/automated/check-virtual-binding-fallback-inputs.py`.
