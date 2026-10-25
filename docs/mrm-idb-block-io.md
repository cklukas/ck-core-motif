---
nav_exclude: true
---
# Mrm IDB block I/O

IDB block reads and writes now retry interrupted system calls and continue
short transfers until the complete fixed-size record is processed. A zero or
other unrecoverable result still returns `MrmFAILURE`.

The source-level regression is `tests/automated/check-mrm-idb-block-io.py`.
