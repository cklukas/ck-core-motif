# Mrm context resize bounds

`UrmResizeResourceContext` now rejects negative requested sizes before it
applies its minimum-buffer rule. A negative value is invalid input; it must not
silently become a successful minimum-size allocation. The focused regression is
`tests/automated/check-mrm-context-resize-bounds.py`.

An equal-size request is also a no-op: existing sufficient storage is retained
instead of performing an unnecessary allocation that could fail.
