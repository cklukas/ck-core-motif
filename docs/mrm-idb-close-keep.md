---
nav_exclude: true
---
# Mrm IDB close keep/discard behavior

`UrmIdbCloseFile` now honors its `keep_new_file` argument. Write-access files
update their header and remain on disk only when the flag is true; successful
discard closes delete the working file. Read-only closes continue to preserve
the source file.

The source-level regression is `tests/automated/check-mrm-idb-close-keep.py`.
