---
nav_exclude: true
---
# Mrm index-writer input validation

The low-level leaf and node index insertion routines now reject null file,
record-buffer, or index pointers before reading the index length. Valid index
insertion and record-splitting behavior remain unchanged; malformed writer
input returns `MrmFAILURE` instead of faulting in `strlen`.

The source-level regression is
`tests/automated/check-mrm-index-writer-input.py`.
