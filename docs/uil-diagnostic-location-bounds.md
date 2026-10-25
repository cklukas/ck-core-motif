---
nav_exclude: true
---
# UIL diagnostic location bounds

UIL diagnostics include the source file name in a fixed 132-byte location
line.  A long source path or a localized format string can exceed that
capacity.  The location variants now use `snprintf` with the destination
size, preserving the normal diagnostic text while truncating only the display
line at the established listing boundary.

`tests/automated/check-uil-diagnostic-location-bounds.py` protects all source
location variants, including catalog and non-catalog builds.
