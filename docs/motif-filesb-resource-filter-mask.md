---
nav_exclude: true
---
# FileSB resource-change filter-mask allocation failure

When directory navigation updates the File Selection Box resources, the full
path filter field is rebuilt from temporary directory and pattern strings. The
combined length is now checked against both `size_t` and Xt's `Cardinal` width
before copying or updating the text field. If sizing or allocation fails, the
existing filter text remains unchanged and the source strings are still
released.

The regression check is
`tests/automated/check-filesb-resource-filter-mask.py`.
