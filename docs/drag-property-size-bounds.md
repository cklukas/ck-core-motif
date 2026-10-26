---
nav_exclude: true
---

# Bound DnD receiver property sizes

The Motif drag receiver protocol stores its heap offset in a 32-bit field and
passes property lengths to Xlib as `int`.  `_XmSetDragReceiverInfo` now rejects
buffers that cannot be represented in either field, releases temporary heap
storage, and publishes only checked lengths.  This prevents truncation of a
large drop-site tree and keeps the receiver's property layout coherent.
