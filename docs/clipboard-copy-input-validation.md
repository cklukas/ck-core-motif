# Clipboard copy input validation

`XmClipboardCopy` now rejects missing format names and null non-empty data
buffers.  `XmClipboardCopyByName` rejects null non-empty data buffers as well.
Both return `ClipboardBadFormat` before clipboard state changes; valid and
zero-length copy operations remain compatible.
