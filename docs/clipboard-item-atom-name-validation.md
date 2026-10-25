---
nav_exclude: true
---

# Clipboard item atom-name validation

Clipboard item IDs are encoded into root-property atom names. The helper now
rejects negative IDs and checks bounded formatting before interning the name,
so malformed IDs cannot produce truncated or overflowing atom names. Existing
header, next-ID, and valid item-ID names remain unchanged.

The focused regression check is
`tests/automated/check-clipboard-item-atom-name-validation.py`.
