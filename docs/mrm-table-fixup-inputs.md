---
nav_exclude: true
---
# Mrm compression-table fixup bounds

`Urm__FixupCompressionTable` converts file offsets into in-memory pointers before
class and resource lookups use a compression table. It now rejects a null table
and a table whose entry count cannot contain the reserved entries before
walking its entries. Valid tables follow the existing byte-swap and fixup path
unchanged.
