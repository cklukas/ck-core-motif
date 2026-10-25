# XmScreen scratch-pixmap cache failures

Scratch-pixmap checkout and return now check `_XmAddHashEntry`.  If the in-use
or scratch cache cannot allocate a hash bucket, the pixmap and ownership key
are released instead of becoming unreachable and leaking.  A failed checkout
returns `None`.

The focused regression is
`tests/automated/check-screen-scratch-cache-failure.py`.
