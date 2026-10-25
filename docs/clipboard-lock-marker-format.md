---
nav_exclude: true
---

# Clipboard lock-marker format validation

The lock-access marker is written as an 8-bit property. `ClipboardWindowExists`
now requires that format when checking a surviving lock window, so a stale
property with an incompatible X11 representation cannot be mistaken for a
valid Motif lock marker.

The focused regression check is
`tests/automated/check-clipboard-lock-marker-format.py`.
