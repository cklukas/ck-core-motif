# WSM per-attribute list allocation failure

WSM 8, 16, and 32-bit list attributes now check their data-array allocations
before reading wire values. A non-empty allocation failure resets the record to
`WSM_NONE` with zero length, allowing enclosing cleanup to proceed safely.
Empty lists retain their valid zero-length representation.

`tests/automated/check-wsm-single-data-oom.py` protects all three paths.
