---
nav_exclude: true
---
# Drag atom wire-width validation

`WriteAtomsTable` serializes X11 `Atom` and `Time` values into Motif's
32-bit atom-table record. Values that cannot be represented by that wire
format are now rejected instead of being silently truncated on 64-bit
platforms. The writer also makes the already-bounded allocation and header
field conversions explicit.

The focused regression is
`tests/automated/check-drag-property-length-bounds.py`.
