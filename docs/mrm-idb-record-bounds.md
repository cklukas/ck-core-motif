---
nav_exclude: true
---
# Mrm in-memory IDB record bounds

In-memory IDB record access now rejects nonpositive record numbers and records
beyond the header's logical last-record value before calculating a UID-buffer
offset. The header record remains available during initial open validation.

The source-level regression is `tests/automated/check-mrm-idb-record-bounds.py`.
