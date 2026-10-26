---
nav_exclude: true
---

# Validate raw XPM-image output records

`XpmReadFileToXpmImage` now rejects a null filename, `XpmImage`, or
`XpmInfo` pointer before initializing caller-owned output records.  This keeps
the raw parser's invalid-input behavior deterministic and prevents a malformed
caller from crashing in the initialization helpers.
