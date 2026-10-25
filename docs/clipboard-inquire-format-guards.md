# Clipboard format inquiry guards

`XmClipboardInquireFormat` now rejects a null `outlength` pointer before
locking and checks the clipboard header before using its retrieval state.
Invalid calls return `ClipboardBadFormat`; header retrieval failure releases
locks and returns `ClipboardFail`.
