---
nav_exclude: true
---

# UIL byte-table bounds

The UIL database reader allocates byte tables from the record's advertised
`table_size` and then reads the advertised item count.  Each byte-table record
now verifies that `num_items` fits within `table_size` before reading.  A
malformed UID record is rejected with the normal bad-database diagnostic,
without changing valid database compatibility.
