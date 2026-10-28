---
nav_exclude: true
---

Avoid double-free during scaled-image rollback
===============================================

When scaled-pixmap construction fails after temporarily replacing an image
buffer, rollback restores the caller-owned `old_image_data`.  The failure
cleanup now detaches that restored pointer before destroying the `XImage`, then
frees it once with Xt.  Converted buffers continue to use the matching
allocator path.  This prevents a double-free in the uncached failure path.

The focused regression is:

```text
python3 tests/automated/check-image-converted-cleanup.py
```
