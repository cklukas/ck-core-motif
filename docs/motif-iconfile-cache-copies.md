---
nav_exclude: true
---
# Cached icon path copies

When the icon-file cache is enabled, directory and leaf names are copied into
the cache record.  Each copy is now checked; a failure releases all partial
fields and skips optional caching without affecting the resolved icon path.

The regression check is
`tests/automated/check-iconfile-cache-copies.py`.
