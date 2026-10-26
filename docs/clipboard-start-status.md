---
nav_exclude: true
---
# Clipboard-start status handling

`XmeClipboardSource` now requires `XmClipboardStartCopy` to return
`XmClipboardSuccess` before storing or using the returned item ID.  All other
failure statuses—including lock, bad format, and internal failures—return
`False` without entering the copy protocol with an uninitialized ID.

The focused regression is
`tests/automated/check-clipboard-start-status.py`.
