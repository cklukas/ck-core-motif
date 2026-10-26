---
nav_exclude: true
---
# Drag atom ordering

The DragBS target-table sort comparator now orders `Atom` values with relational
comparisons instead of subtracting them into an `int`. Large 64-bit atom IDs
therefore cannot overflow the comparator or be mistaken for equal values.

The focused regression is
`tests/automated/check-drag-property-length-bounds.py`.
