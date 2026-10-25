# Image installation failure rollback

`_XmInstallImage` now checks the hash-table insertion result. If bucket
allocation fails, it frees the copied image name and entry and reports failure
instead of claiming the image was installed while leaving an unreachable
allocation behind.

The focused regression is `tests/automated/check-image-install-failure.py`.
