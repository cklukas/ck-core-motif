---
nav_exclude: true
---
# Xm stippled-pixmap cache serialization

`XiCreateStippledPixmap` now locks cache lookup and rechecks the cache before
publishing a newly created entry. Concurrent callers share one matching
pixmap; a duplicate created during the race is freed immediately.

The source-level regression is
`tests/automated/check-xm-stippled-pixmap-race.py`.
