---
nav_exclude: true
---
# Mrm header version parsing

`Idb__HDR_GetHeader` now parses both the on-disk and compiled-in database
version strings with a range-checked `strtol` helper. Malformed, negative, or
out-of-range major/minor values are rejected without invoking undefined
`sscanf` integer conversion behavior, and the compiled-in string no longer
needs a temporary allocation.

The source-level regression is
`tests/automated/check-mrm-header-version-allocation.py`.
