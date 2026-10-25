---
nav_exclude: true
---

# WSM reply packing failure handling

`WSMProcessProtoTarget` now initializes conversion outputs before processing a
  request and checks the result of `_WSMPackReply`. A failed non-empty reply
  releases request/reply state and returns `False` instead of reporting a
  successful conversion with a null buffer. Valid replies retain their wire
  format and target type.

The focused regression check is
`tests/automated/check-wsm-reply-packing-guards.py`.
