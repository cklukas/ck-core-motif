---
nav_exclude: true
---
# Mrm hierarchy file-list bounds

Mrm hierarchy opening now rejects negative file counts and checks the file-list
byte multiplication against `SIZE_MAX` before allocating the primary and group
file vectors. The byte-size variable uses `size_t`, avoiding truncation through
the legacy `MrmCount` type while preserving valid hierarchy behavior.

The source-level regression is `tests/automated/check-mrm-hierarchy-counts.py`.
