# Clipboard copy-by-name validation

`XmClipboardCopyByName` now validates the clipboard header, format record, and
format-data retrieval before dereferencing or appending data. Invalid results
release temporary records and any acquired lock and return `ClipboardFail`.
Zero-length copy-by-name operations remain valid without pointer arithmetic on
a null data buffer.
