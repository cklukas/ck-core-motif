---
nav_exclude: true
---
# Mrm IDB close-error delete mode

The IDB close-error path now deletes the underlying file only for write-access
handles. Read-only files are closed and released without being unlinked when a
flush or header operation fails.

The source-level regression is
`tests/automated/check-mrm-idb-close-delete-mode.py`.
