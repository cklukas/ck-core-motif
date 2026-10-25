---
nav_exclude: true
---
# Pixmap creation failure

A failed `XCreatePixmap` returned the unspecified handle but the old path still
inserted that handle into both pixmap caches and attempted to upload into it.
This leaked the temporary cache entry and image/color ownership and exposed a
bad resource to later callers.

The failure path now restores any temporary depth-1 image conversion, releases
all owned allocations, and returns before cache insertion.  Valid pixmap
creation is unchanged.

`tests/automated/check-pixmap-create-failure.py` protects this contract.
