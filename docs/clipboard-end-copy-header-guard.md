# Clipboard end-copy header guard

`XmClipboardEndCopy` now checks the header returned by `ClipboardOpen` before
reading its copy state. If header retrieval fails, the operation releases the
clipboard lock and application mutex and returns `ClipboardFail`; successful
copy finalization is unchanged.
