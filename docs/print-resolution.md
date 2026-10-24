# PrintShell resolution parsing

The PrintShell resource update path accepts the printer server's
`default-printer-resolution` string. It now handles a missing, non-positive,
malformed, or overflowing value by returning zero, which preserves the
existing fallback to the X screen resolution. Valid decimal prefixes such as
`300dpi` remain accepted for compatibility.

Run the installed-library regression with:

```sh
python3 tests/automated/run-print-resolution-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
