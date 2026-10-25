# Clipboard callback table growth

Cut-by-name callbacks use paired callback and item-id tables. `XmClipboardStartCopy`
now grows those tables with checked allocate-copy-swap logic. A failed
allocation leaves the existing pair untouched, releases the temporary item and
clipboard lock, restores the previous header state, and returns
`ClipboardFail`; successful callback registration remains unchanged.
