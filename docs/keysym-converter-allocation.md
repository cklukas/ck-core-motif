---
nav_exclude: true
---
# KeySym resource conversion allocation

The string-to-KeySym-table resource converter allocates a table and a mutable
copy of the input string. Both allocations are now checked. If either fails,
the converter returns `False` and releases any table already allocated, while
successful parsing and invalid-name cleanup remain unchanged. Input length and
comma-count growth are validated before either Xt `Cardinal` allocation, so an
unrepresentable resource string cannot truncate the mutable copy or table.
