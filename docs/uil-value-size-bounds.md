# UIL value-size arithmetic

UIL resource emission accumulates string-table, font-table, integer-table, and
translation-table payload sizes in an `int` before resizing the resource
context. Large or malformed tables could wrap that counter and cause an
undersized allocation followed by out-of-bounds writes.

`UilP2Out.c` now uses a checked size-add helper for those accumulations. An
unrepresentable resource reports the existing internal error and stops
emission, while valid resources retain the same layout and offsets. The source
regression `tests/automated/check-uil-value-size-bounds.py` covers the helper
and its table call sites.
