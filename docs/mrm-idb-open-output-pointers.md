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

The low-level file opener follows the same contract.  Once its output pointer
has been validated, it clears that output before inspecting or opening the
requested pathname.  A failed open therefore cannot leave a caller holding a
stale low-level file handle.
