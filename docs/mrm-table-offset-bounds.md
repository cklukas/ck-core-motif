---
nav_exclude: true
---
# Mrm compression-table offset bounds

Compression-table entries contain offsets into the table's trailing string
storage. Fixup now computes the minimum byte offset after the complete entry
array and rejects entries that point back into the header or entry metadata.
This prevents malformed UID data from treating table metadata as a string while
leaving valid string offsets unchanged.
