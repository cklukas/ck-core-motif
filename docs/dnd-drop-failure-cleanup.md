---
nav_exclude: true
---

# DnD drop failure cleanup

Text and TextField allocate a local drop-transfer record before inspecting an
external owner's targets. Unsupported operations and missing targets now free
that record, complete the transfer with failure, and return immediately. This
avoids both a leak and continued work after transfer completion.

The regression check is
`tests/automated/check-dnd-drop-failure-cleanup.py`.
