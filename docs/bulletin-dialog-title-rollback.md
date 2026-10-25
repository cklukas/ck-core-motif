---
nav_exclude: true
---
# BulletinBoard dialog-title rollback

`XmBulletinBoard` now copies a changed dialog title before releasing the
current `XmString`. Allocation failure preserves the existing window-manager
title; NULL still explicitly clears it.

The focused source regression is
`tests/automated/check-bulletin-dialog-title-rollback.py`.
