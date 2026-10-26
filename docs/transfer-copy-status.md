---
nav_exclude: true
---
# Clipboard copy-status bookkeeping

Clipboard transfer setup can reject a format or length after the transfer
code has requested the copy.  The transfer code now increments its successful
count only when `XmClipboardCopy` returns `ClipboardSuccess`; deferred
snapshot requests are added to the callback dictionary only after the same
check.  Failed requests therefore cannot trigger a false successful MOVE or
leave an unresolvable callback entry behind.  Invalid selection formats are
also rejected before byte-length conversion.

The focused regression is
`tests/automated/check-transfer-copy-status.py`.
