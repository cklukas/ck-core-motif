---
nav_exclude: true
---

# Bound DnD buffer reads

`_XmReadDragBuffer` now performs its cursor and remaining-size arithmetic in
`size_t` and uses subtraction-based bounds checks.  A wrapped `numCurr + size`
calculation can no longer bypass the clamp and copy past the received property
buffer.
The drag-property read and write helpers reject a null property buffer, and a
null payload when the requested transfer is nonzero, before copying bytes.
Zero-length transfers remain harmless no-ops.  This protects malformed or
failed conversion paths while preserving valid transfer behavior.
