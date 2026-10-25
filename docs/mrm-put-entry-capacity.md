---
nav_exclude: true
---
# Mrm data-entry insertion capacity

Before adding a simple entry to an existing data record, Mrm now validates the
record's `free_ptr` and `free_count` against the fixed payload size and checks
that their combined range remains inside the record. The check is repeated
after allocating a replacement record, so malformed on-disk metadata cannot
make the writer form an out-of-bounds destination pointer.
