# Clipboard format inquiry zero-length results

`XmClipboardInquireFormat` now skips the destination copy when the effective
buffer length is zero. This keeps the supported `buffer == NULL,
bufferlength == 0` query form safe while preserving truncation and normal
non-empty copies.
