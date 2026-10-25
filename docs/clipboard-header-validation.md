# Clipboard header validation

`ClipboardOpen` now rejects short or null clipboard-header properties before
callers access header fields. A malformed header property is removed and a
fresh default header is retrieved, preserving normal clipboard recovery while
avoiding out-of-bounds header reads.
