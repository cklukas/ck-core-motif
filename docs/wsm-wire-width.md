---
nav_exclude: true
---

# WSM property wire width

WM-window properties are serialized with `PackProperty` as 32-bit protocol
fields.  Request and reply size calculations now use `sizeof(CARD32)` rather
than host `Window`/`WindowProperty` widths, preventing oversized messages and
uninitialized padding from being sent on 64-bit platforms.

All list and string lengths serialized through `PackListNum` are now bounded
to the protocol's `CARD16` maximum before packing. Oversized outbound records
are rejected rather than being truncated in the wire header.

The background-window request's screen number uses the same `CARD16` field and
is likewise validated before its explicit wire conversion.
The focused check normalizes source whitespace and accepts the explicit size
casts used by the implementation.
