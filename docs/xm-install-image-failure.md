# Xm image-install failure handling

`_XmInstallImage` now returns `False` when lazy image-table initialization,
`ImageData` allocation, or image-name duplication fails. Partial records are
freed before unlocking, and successful installs retain the existing duplicate
checking and caller-owned `XImage` behavior.

The source-level regression is `tests/automated/check-xm-install-image-failure.py`.
