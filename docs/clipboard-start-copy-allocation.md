# Clipboard start-copy allocation failure handling

`XmClipboardStartCopy` now checks the data-item allocation before updating the
clipboard header or dereferencing the new item. If allocation fails, it closes
the header, releases the clipboard lock, and returns `ClipboardFail` without
publishing a partial copy transaction.
