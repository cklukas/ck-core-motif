---
nav_exclude: true
---

# Complete color-cache properties

Color-selection cache discovery now rejects a property when X reports
remaining bytes after the requested read.  This prevents an oversized or
truncated cache record from entering pixel-data parsing; malformed data still
follows the existing cleanup and cache-miss path.

Regression: `tests/automated/check-color-cache-property-complete.py`
