---
nav_exclude: true
---
# XmInstallImage locking

`XmInstallImage` now delegates directly to `_XmInstallImage`, which owns the
process-locking and allocation contract. The public wrapper no longer acquires
the same process lock around the synchronized helper, avoiding a deadlock when
Xt is configured with a non-recursive process lock.

The focused regression is `tests/automated/check-xm-install-locking.py`.
