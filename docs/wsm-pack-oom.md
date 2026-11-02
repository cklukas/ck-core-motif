---
nav_exclude: true
---

# WSM message packing allocation

WSM request and reply builders now check their output-buffer allocation before
packing fields into it.  A non-empty message returns `NULL` on allocation
failure, while zero-length message types retain their existing behavior.  The
wire format and successful packing paths are unchanged.

The computed request and reply sizes are accumulated through checked helpers;
extension and configuration-format records no longer use unchecked repeated
addition.  The final value is verified as representable by Xt's `Cardinal`
before allocation, so an oversized message fails before a narrowed allocation
can under-allocate its wire buffer.

`tests/automated/check-wsm-pack-size-bounds.py` protects this contract.
