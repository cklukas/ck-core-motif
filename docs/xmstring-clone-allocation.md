---
nav_exclude: true
---
# XmString clone allocation

Both optimized and non-optimized `XmStringCopy` clones validate allocation
sizes and results before copying. The non-optimized path releases every entry
copied so far when a child copy fails; the optimized path rejects wrapped
payload sizes and failed allocation. Neither path publishes a partial clone.

The focused regression check is
`tests/automated/check-xmstring-clone-allocation.py`.
