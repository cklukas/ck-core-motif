---
nav_exclude: true
---
# Clipboard ownership status

When a clipboard manager is present, `XmeClipboardSource` reports the result
of `XtOwnSelection` to its caller.  Ownership failure now returns `False`
instead of incorrectly reporting success, so applications can retry or show
an error without assuming the source is active.

The focused regression is
`tests/automated/check-clipboard-owner-status.py`.
