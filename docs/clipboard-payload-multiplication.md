---
nav_exclude: true
---
# Clipboard payload length conversion

Clipboard converters report element counts while `XmClipboardCopy` consumes a
byte length. The 8/16/32-bit conversion now checks the format unit before
multiplication, discarding an unrepresentable payload rather than allowing the
length to wrap.

The focused regression is
`tests/automated/check-clipboard-payload-multiplication.py`.
