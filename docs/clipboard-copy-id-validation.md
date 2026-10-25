# Clipboard copy ID validation

`XmClipboardCopy` now checks the clipboard header and both newly allocated
format IDs before linking or publishing a format record. A failed ID
allocation releases the temporary format/item records and clipboard lock
without publishing partial state.
