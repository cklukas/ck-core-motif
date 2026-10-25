# Clipboard pending-items header guard

`XmClipboardInquirePendingItems` now checks the clipboard header before deriving
the item-list pointer. Header retrieval failure releases the clipboard lock and
application mutex and returns `ClipboardFail`; the existing null-list shortcut
remains unchanged.
