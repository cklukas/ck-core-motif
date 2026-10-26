---
nav_exclude: true
---
# Drag target wire-width validation

`WriteTargetsTable` serializes target `Atom` values into Motif's 32-bit
target-list records. Values outside that wire representation are now rejected
instead of being silently truncated on 64-bit systems. The count, offset, and
allocation conversions are explicit after their existing bounds checks.

The focused regression is
`tests/automated/check-drag-property-length-bounds.py`.
