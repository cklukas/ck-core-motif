---
nav_exclude: true
---

# Clipboard lock-marker property bound

`ClipboardWindowExists` only needs to know whether the lock-access marker
property exists. It now requests one property unit and checks the X11 return
status, avoiding a large allocation for a tiny marker and treating failed or
empty reads as an absent marker.

The focused regression check is
`tests/automated/check-clipboard-lock-marker-request.py`.
