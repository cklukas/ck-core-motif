---
nav_exclude: true
---
# Mrm IDB open output handles

The IDB file and in-memory-buffer open helpers now reject missing output
handles before opening or allocating state. In-memory opens also reject a
missing UID buffer, and the write-open path closes its low-level file cleanly
when descriptor allocation fails.

The source-level regression is
`tests/automated/check-mrm-idb-open-output-pointers.py`.
