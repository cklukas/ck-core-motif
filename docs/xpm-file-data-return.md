---
nav_exclude: true
---
# XPM file-to-data output

`XpmReadFileToData` requires a destination pointer for its allocated data
table. A null output pointer is rejected before reading the file or invoking
the data-construction path.

The source contract is covered by
`tests/automated/check-xpm-file-data-return.py`.
