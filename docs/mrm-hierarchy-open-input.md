---
nav_exclude: true
---
# Mrm hierarchy-open input validation

Hierarchy opening rejects a missing output pointer, a missing filename list
when files are requested, and a missing in-memory UID buffer before opening
displays or allocating hierarchy state. After the output pointer is validated,
it is cleared before any failure-prone validation or allocation so a failed
open cannot leave a stale hierarchy handle. Valid file-backed and in-memory
opens retain their existing behavior.

The source-level regression is
`tests/automated/check-mrm-hierarchy-open-input.py`.
