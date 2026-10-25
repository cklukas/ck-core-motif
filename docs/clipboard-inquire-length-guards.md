# Clipboard length inquiry guards

`XmClipboardInquireLength` now rejects a null format name before calling X11
atom APIs and checks the clipboard header before accessing retrieval state.
Header failure releases the clipboard lock and application mutex; the optional
`length` output remains optional as before.
