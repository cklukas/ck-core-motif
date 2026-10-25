---
nav_exclude: true
---
# Mrm index-record copy inputs

The leaf and node index-record copy helpers now return immediately when either
record pointer is missing.  This keeps failed record allocations from turning
index maintenance into a null-pointer crash while preserving the existing
fixed-size copy behavior for valid records.

The source-level regression is
`tests/automated/check-mrm-index-record-copies.py`.
