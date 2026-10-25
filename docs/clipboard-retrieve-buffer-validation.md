# Clipboard retrieve buffer validation

`XmClipboardRetrieve` now rejects a missing format name and a null output
buffer when a nonzero length is requested, returning `ClipboardBadFormat`
before clipboard access.  Valid retrieval and zero-length queries retain their
existing behavior.
