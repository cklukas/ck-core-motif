---
nav_exclude: true
---

# Transfer location cleanup

Container, Text, and TextField register completion callbacks to release
temporary drag-location data. Those callbacks now tolerate a missing
completion structure, a stale transfer ID, or an already-cleared location.
This keeps cancellation, destruction, and reentrant completion paths from
turning cleanup into a null dereference or repeated free.

The regression check is
`tests/automated/check-transfer-location-cleanup.py`.
