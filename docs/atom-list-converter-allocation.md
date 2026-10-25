---
nav_exclude: true
---
# Atom-list resource conversion allocation

The atom-list converter now checks dynamic token-array growth and the final
atom-array allocation against both host `size_t` and Xt's `Cardinal` byte
range. Any failure releases token strings and heap-backed name storage before
returning `False`; successful `XInternAtoms` conversion and output ownership
remain unchanged.
