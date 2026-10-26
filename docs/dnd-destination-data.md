---
nav_exclude: true
---

# DnD destination callback data

Drop callbacks from an external owner may be cancelled or arrive after the
destination data has been cleared. Container, Text, and TextField now reject
missing `XmDropProcCallbackStruct` data and complete the transfer with
`XmTRANSFER_DONE_FAIL` before reading coordinates or the drop action.

The regression check is
`tests/automated/check-dnd-destination-data.py`.
