---
nav_exclude: true
---

# Keep DnD write offsets representable

`_XmWriteDragBuffer` retains its `CARD16` ABI return type but now saturates
offsets above `UINT16_MAX` instead of wrapping them.  The full buffer remains
available to the protocol writer; only the legacy diagnostic/result offset is
bounded.
