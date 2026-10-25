---
nav_exclude: true
---
# Mrm data-entry writer inputs and sizes

`Idb__DB_PutDataEntry` now rejects a missing file handle or output data handle
before calling the header or buffer writers. Its record-size arithmetic also
uses unsigned/wider locals for entry, remaining, and segment sizes, avoiding
signed 16-bit overflow while retaining the existing 16-bit on-disk fields and
public API.
