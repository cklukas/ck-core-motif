---
nav_exclude: true
---
# Mrm IDB write-open conflict cleanup

When a write-open finds an existing file but cannot proceed because the OS
extension is missing, clobbering is disabled, or the extension version is
invalid, the already-open descriptor is now closed before returning. This
prevents descriptor leaks while preserving each existing status code.

The source-level regression is
`tests/automated/check-mrm-idb-write-conflict-cleanup.py`.
