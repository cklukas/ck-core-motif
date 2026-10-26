---
nav_exclude: true
---

# DnD transfer callback inputs

Text and TextField drop-transfer callbacks now reject missing widgets,
selection callback structures, closures, and transfer records before
dereferencing them. Their completion callbacks also tolerate null widgets or
callback data during teardown. Invalid requests complete with failure when a
transfer ID is available.

The regression check is
`tests/automated/check-dnd-transfer-callback-inputs.py`.
