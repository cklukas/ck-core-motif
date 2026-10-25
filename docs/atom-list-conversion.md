---
nav_exclude: true
---
# Atom-list resource conversion

`XmRAtomList` conversion now checks temporary name-array growth and the final
atom-array size before multiplying allocation counts. If a malformed resource
would exceed representable storage, all token strings and temporary arrays are
released and conversion fails cleanly. Accepted name-array and atom-array byte
counts are passed explicitly as Xt `Cardinal` values.

The source-level capacity regression is
`tests/automated/check-atom-list-bounds.py`.
