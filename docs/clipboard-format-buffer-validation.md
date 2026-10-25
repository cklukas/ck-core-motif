# Clipboard format buffer validation

`XmClipboardInquireFormat` now rejects a null output buffer when a nonzero
buffer length is requested, returning `ClipboardBadFormat` before clipboard
locking or copying.  Zero-length inquiries and valid buffers retain their
existing behavior.
