---
nav_exclude: true
---
# KeySym resource conversion allocation

The string-to-KeySym-table resource converter allocates a table and a mutable
copy of the input string. Both allocations are now checked. If either fails,
the converter returns `False` and releases any table already allocated, while
successful parsing and invalid-name cleanup remain unchanged.
