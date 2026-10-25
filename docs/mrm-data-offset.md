---
nav_exclude: true
---
# Mrm data-entry offset validation

Data handles carry a record number and an offset to an entry inside the fixed
size data-record payload. The readers now reject offsets that cannot even hold
a complete `IDBDataEntryHdr` before forming the entry pointer. This prevents
malformed handles or corrupted files from indexing past the record while
preserving all valid handles produced by the writer.
