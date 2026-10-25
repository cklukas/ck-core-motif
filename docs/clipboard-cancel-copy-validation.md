# Clipboard cancel-copy validation

`XmClipboardCancelCopy` now validates the next-id property before restoring it
and checks the clipboard header before changing its copy state. Missing,
failed, or undersized properties release the clipboard lock and return
`ClipboardFail` without dereferencing invalid storage.
