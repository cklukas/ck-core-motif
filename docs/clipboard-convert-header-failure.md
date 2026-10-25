---
nav_exclude: true
---

# Clipboard conversion header failure handling

Clipboard header creation can fail when serialized clipboard storage cannot be
allocated. The conversion callback and next-item ID helper now stop cleanly
when `ClipboardOpen` returns no header instead of dereferencing a null pointer.
Normal clipboard conversion and ID updates are unchanged.

The focused regression check is
`tests/automated/check-clipboard-convert-header-failure.py`.
