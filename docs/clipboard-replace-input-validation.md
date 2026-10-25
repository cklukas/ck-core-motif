---
nav_exclude: true
---

# Clipboard property replacement input validation

`ClipboardReplaceItem` converts byte lengths into X11 property units. It now
rejects formats other than 8, 16, and 32 bits, null buffers for non-empty
properties, and lengths that are not an exact number of format units before
performing that conversion. Valid replacements retain their existing chunking
and ownership behavior.

The focused regression check is
`tests/automated/check-clipboard-replace-input-validation.py`.
