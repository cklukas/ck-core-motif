# Bound automatic drag `TARGETS` conversion

When a drag source does not provide its own `TARGETS` conversion, Motif builds
the atom list automatically.  Both normal and incremental conversion paths
now check the atom-count multiplication against the `int`/Xt allocation
contract, reject missing source arrays, and handle allocation failure without
calling `memmove` on a null pointer.

Zero-target sources continue to return an empty successful conversion.  The
focused source regression is
`tests/automated/check-drag-targets-conversion-bounds.py`.
