# Mrm compression-table offset bounds

Compression-table fixup now checks the table-entry allocation size before
walking entries and rejects string offsets that point into the table header or
entry array. Valid offsets continue to resolve to in-table strings through the
existing byte-swap and lookup path.

The source-level regression check is
`tests/automated/check-mrm-table-offset-bounds.py`.
