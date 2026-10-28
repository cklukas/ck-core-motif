---
nav_exclude: true
---
# PrintShell resolution parsing

The PrintShell resource update path and ImageCache's print-scaling path accept
the printer server's `default-printer-resolution` string. Both use the same
checked parser: a missing, non-positive, malformed, or overflowing value
returns zero, preserving the existing fallback behavior. Valid decimal
prefixes such as `300dpi` remain accepted for compatibility, and the X Print
attribute returned to ImageCache is released after parsing.

Run the installed-library regression with:

```sh
python3 tests/automated/run-print-resolution-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
