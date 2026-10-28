---
nav_exclude: true
---

# Clipboard label ID bounds

`ClipboardDeleteItemLabel` now rejects serialized label IDs above `LONG_MAX`
before converting the unsigned record field to Motif’s signed `itemId` type.
Malformed clipboard records can no longer wrap into a different deletion ID.

Regression: `tests/automated/check-clipboard-label-id-bounds.py`
