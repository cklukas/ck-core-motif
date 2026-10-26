---
nav_exclude: true
---

# Image uninstall locking

`XmUninstallImage` serializes access to the image cache while it maps the
cache and removes the matching entry. The map callback therefore relies on
that caller-held `_XmProcessLock`; it must not acquire the same lock again.
This avoids a nested-lock deadlock on builds whose process lock is not
recursive, while preserving the existing cache serialization.

The regression check is
`tests/automated/check-image-uninstall-locking.py`.
