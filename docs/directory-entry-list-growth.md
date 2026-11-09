---
nav_exclude: true
---
# Directory-entry list growth bounds

The file-selection directory scanner grows its returned entry-pointer list in
fixed increments. Before reallocating, it now validates both native `size_t`
bytes and Xt's legacy `Cardinal` allocation range. An unrepresentably large
entry list stops scanning cleanly instead of truncating the reallocation size.

The focused regression check is
`tests/automated/check-directory-entry-list-growth.py`.
