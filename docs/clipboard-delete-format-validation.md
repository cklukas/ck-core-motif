---
nav_exclude: true
---

# Clipboard delete-format record validation

`ClipboardDeleteFormat` reads serialized format and data-item records while
marking a cut-by-name format as cancelled. It now checks lookup status and
record sizes before reading either record, and releases the format record if
the parent data item is unavailable or malformed. Valid clipboard records keep
the existing cancellation and cleanup behavior.

The focused regression check is
`tests/automated/check-clipboard-delete-format-validation.py`.
