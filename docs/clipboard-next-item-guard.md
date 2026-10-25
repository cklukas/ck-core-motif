# Clipboard next-item update guard

`ClipboardSetNextItemId` now checks the result of retrieving the next-item
property before writing through it. Missing, failed, or undersized properties
are released and ignored, avoiding a null or short-buffer dereference while
leaving the clipboard property unchanged.
