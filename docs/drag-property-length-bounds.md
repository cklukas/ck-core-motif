---
nav_exclude: true
---

# Bound DragBS property lengths

The atom and target-list property writers now check both protocol storage
(`CARD32`) and Xlib's signed property-length parameter before publishing a
buffer.  Explicit casts occur only after those checks, preventing large tables
from being silently truncated on 64-bit hosts.
