# Clipboard undo-copy validation

`XmClipboardUndoCopy` now checks the clipboard header and the last-copy data
record before reading or mutating them. Missing, failed, or undersized records
release temporary storage and clipboard locks and return `ClipboardFail`, while
valid undo operations retain their existing behavior.
