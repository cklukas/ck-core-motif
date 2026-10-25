---
nav_exclude: true
---

# XmString optimized clone allocation

Optimized `XmStringCopy` clones now validate the computed payload size and
handle allocation failure before copying. This prevents arithmetic wraparound
and null-pointer dereferences for malformed or allocation-starved strings.

The focused regression check is
`tests/automated/check-xmstring-clone-allocation.py`.
