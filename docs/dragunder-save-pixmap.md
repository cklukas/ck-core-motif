---
nav_exclude: true
---

# Drag-under scratch pixmap allocation

`SaveAll` now validates the scratch pixmap before issuing `XCopyArea`. A
failed or unspecified pixmap releases the saved-record allocation and resets
the cleanup count, so animation code reports failure without sending an X
request to an invalid drawable.

The focused source contract is checked by
`tests/automated/check-dragunder-save-pixmap.py`.
