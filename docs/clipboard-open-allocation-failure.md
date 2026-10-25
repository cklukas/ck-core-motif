---
nav_exclude: true
---

# Clipboard header allocation failure handling

`ClipboardOpen` may need to create a header record when the clipboard property
is absent or malformed. If allocation fails, the helper now returns `NULL`
instead of initializing through a null pointer. Existing callers can release
their lock and report the operation failure, while successful clipboard opens
retain the existing behavior.

The focused regression check is
`tests/automated/check-clipboard-open-allocation-failure.py`.
