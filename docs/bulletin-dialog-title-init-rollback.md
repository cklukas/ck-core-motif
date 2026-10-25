---
nav_exclude: true
---
# BulletinBoard dialog-title initialization rollback

`XmBulletinBoard` now checks the private copy of an initial dialog title
before publishing it to the parent shell.  If allocation fails, the existing
resource value remains in place and the window-manager title is not updated
with NULL.  Changed-title `SetValues` rollback behavior remains unchanged.

The focused regression is
`tests/automated/check-bulletin-dialog-title-init-rollback.py`.
