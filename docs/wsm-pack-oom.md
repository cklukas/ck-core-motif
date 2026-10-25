---
nav_exclude: true
---

# WSM message packing allocation

WSM request and reply builders now check their output-buffer allocation before
packing fields into it.  A non-empty message returns `NULL` on allocation
failure, while zero-length message types retain their existing behavior.  The
wire format and successful packing paths are unchanged.
