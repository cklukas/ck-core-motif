---
nav_exclude: true
---

# Clipboard format enumeration bounds

`ClipboardFindFormat` walks a serialized data-item record and its format-ID
list. It now validates the header, record length, list offset/count, and each
format record before reading fields. Malformed clipboard properties are
rejected and released without changing enumeration of valid records.

The focused regression check is
`tests/automated/check-clipboard-format-enumeration-bounds.py`.
