# Clipboard pending-list allocation handling

`XmClipboardInquirePendingItems` now treats an empty clipboard as a valid
zero-length result without requiring an allocation. For non-empty clipboards it
checks the pending-list size calculation and allocation before iterating, then
closes the header, releases the lock, and returns `ClipboardFail` if the list
cannot be created.
