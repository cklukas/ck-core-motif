---
nav_exclude: true
---
# Mrm writer lookup error propagation

Writer APIs now continue only when their pre-insert lookup reports the
documented absence condition. Indexed-resource insertion accepts
`MrmINDEX_GT` or `MrmINDEX_LT`; RID insertion accepts `MrmNULL_DATA`. Other
lookup failures, including malformed records and I/O errors, are returned
without creating a new data entry.
