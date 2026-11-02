---
nav_exclude: true
---
# Button-type resource conversion allocation

The string-to-button-type converter allocates a result table and a mutable
copy of the input. Both allocations are now checked. If the input copy fails,
the result table is released before returning `False`; successful parsing and
invalid-name cleanup retain the existing behavior. A malformed token sequence
(for example, an empty trailing token) now also releases both the table and
that mutable input copy before returning `False`.
