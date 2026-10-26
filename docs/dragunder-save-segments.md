---
nav_exclude: true
---
# Drag-under segment backing stores

`SaveSegments` now validates every scratch pixmap before copying window pixels
into it. If any strip cannot be allocated, already-created strips are released,
the saved-pixmap list is cleared, and the caller reports that no highlight or
shadow was drawn. This keeps partial backing stores from leaking and prevents
`XCopyArea` from receiving an invalid drawable.

The focused source contract is checked by
`tests/automated/check-dragunder-save-segments.py`.
