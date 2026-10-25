---
nav_exclude: true
---
# Mrm compound-string stream cleanup

`UrmCWR__AppendCString` now releases the temporary byte stream returned by
`XmCvtXmStringToByteStream` after copying it into the resource record.  Empty,
oversized, and failed-resize paths release the temporary allocation as well.

The source-level regression is
`tests/automated/check-mrm-cstring-stream-cleanup.py`.
