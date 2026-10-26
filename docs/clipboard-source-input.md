---
nav_exclude: true
---
# Clipboard-source widget validation

`XmeClipboardSource` now rejects a null widget before deriving the application
context or touching the display.  This matches the guarded primary, named,
secondary, and sink entry points and makes invalid clipboard-source setup a
safe `False` result.

The focused regression is
`tests/automated/check-clipboard-source-input.py`.
