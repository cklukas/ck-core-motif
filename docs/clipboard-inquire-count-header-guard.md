# Clipboard inquire-count header guard

`XmClipboardInquireCount` now checks the header returned by `ClipboardOpen`
before reading retrieval timestamps or querying item lists. Header failure
releases the clipboard lock and application mutex and returns `ClipboardFail`.
