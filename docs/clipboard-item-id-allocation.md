# Clipboard item-ID allocation validation

`ClipboardGetNewItemId` now validates the next-ID property before incrementing
or replacing it. `XmClipboardStartCopy` also aborts cleanly if either the item
or label ID cannot be allocated, restoring the previous header state and
releasing resources.
