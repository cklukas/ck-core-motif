# Clipboard next-id validation

`ClipboardOpen` now validates the `_MOTIF_CLIP_NEXT_ID` property returned from
the root window. A null or undersized value is discarded and replaced with the
standard first-free item ID, keeping subsequent clipboard allocations
consistent after malformed external state.
