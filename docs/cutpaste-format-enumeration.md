# Clipboard format enumeration

`XmClipboardInquireFormat` now skips a target whose `XGetAtomName` lookup
fails instead of passing a null name to `strlen`.  Other target formats remain
enumerated normally, and successful name lookups retain their existing length
calculation.
