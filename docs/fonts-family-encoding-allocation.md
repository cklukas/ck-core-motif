---
nav_exclude: true
---
# Font family encoding allocation handling

Font selector family creation now checks the initial encoding-table allocation.
If it fails, the family keeps an empty encoding table and returns cleanly;
encoding enumeration and matching already treat an empty table as unavailable,
so later font discovery cannot dereference null storage.

The regression check is `tests/automated/check-fonts-family-encoding-allocation.py`.
