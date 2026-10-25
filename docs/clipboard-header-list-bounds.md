---
nav_exclude: true
---

# Clipboard header list bounds

Clipboard headers contain an offset and count for the serialized data-item ID
list. `ClipboardOpen` now validates that the offset fits the property and that
the complete list fits in the retrieved record before returning the header.
Malformed headers are discarded safely; valid headers and their compatibility
layout are unchanged.

The focused regression check is
`tests/automated/check-clipboard-header-list-bounds.py`.
