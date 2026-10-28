---
nav_exclude: true
---

# Mrm compound-string conversion output

`UrmCWR__AppendCString` now rejects an inconsistent converter result where the
encoded length is positive but the returned byte-stream pointer is null. This
prevents a null `memcpy` while preserving empty-string handling and normal
bounded compound-string serialization.
