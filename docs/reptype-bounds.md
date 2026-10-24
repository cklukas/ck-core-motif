# Representation-type table bounds

Dynamic representation-type registration and enumeration check record-count
addition and allocation limits before resizing their tables. If the table
cannot be represented, registration returns `XmREP_TYPE_INVALID` or enumeration
returns `NULL`; ordinary representation types retain their existing IDs and
value mappings.

The source-level regression is `tests/automated/check-reptype-bounds.py`.
