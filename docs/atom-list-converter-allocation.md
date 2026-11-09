---
nav_exclude: true
---
# Atom-list resource conversion allocation

The atom-list converter now checks dynamic token-array growth and the final
atom-array allocation against both host `size_t` and Xt's `Cardinal` byte
range. Any failure releases token strings and heap-backed name storage before
returning `False`. A successful conversion now also releases the temporary
heap-backed name vector used after more than 128 atom names have been parsed;
only the caller-owned atom result remains. `XInternAtoms` conversion and output
ownership remain unchanged.
