---
nav_exclude: true
---
# ScrolledWindow scrollbar size extents

`GetVariableSize` combines scrollbar dimensions with padding and highlight
thickness before using the result in parent-size negotiation.  The sum can
exceed the 16-bit `Dimension` range for large resources; direct assignment
wraps the requested extent and can make the scrolled window too small.

Scrollbar extents now use the bounded extent helper before entering layout
negotiation.  Ordinary sizes are unchanged, while oversized resources clamp to
the representable `Dimension` range.  The contract is protected by
`tests/automated/check-scrolled-size-extents.py`.
