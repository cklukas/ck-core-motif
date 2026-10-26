---
nav_exclude: true
---
# XmInstallImage locking

`XmInstallImage` now delegates directly to `_XmInstallImage`, which owns the
process-locking and allocation contract. The public wrapper no longer acquires
the same process lock around the synchronized helper, avoiding a deadlock when
Xt is configured with a non-recursive process lock.

`XmGetPixmap` follows the same rule: it calls the synchronized scaled-pixmap
helper directly instead of calling `XmGetPixmapByDepth` while holding the
application lock. This prevents a second lock acquisition on the public
compatibility wrapper.

`XmeGetMask` follows the same rule when converting the generated mask name.
It now invokes the synchronized helper directly rather than reacquiring the
application lock through `XmGetPixmapByDepth`.

The focused regressions are `tests/automated/check-xm-install-locking.py` and
`tests/automated/check-xm-get-pixmap-locking.py`.
