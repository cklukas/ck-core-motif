---
nav_exclude: true
---
# UIL database reader failures

The compound WMD table readers now return success only after consuming their
entire record.  The outer loader closes the database immediately when a helper
rejects a count, cannot allocate, or encounters a short read.  It therefore
cannot interpret unread record bytes as another database header.

The same rule applies to the globals and record-header framing: normal end of
file is detected before attempting a header read; a partial header, invalid
header, unknown table ID, or record/count mismatch stops loading immediately.

Zero-byte table payloads are handled without calling `fread` with an empty
element size.  This preserves valid empty records while making incomplete
records fail deterministically.

`tests/automated/check-uil-db-reader-status.py` protects the reader/loader
contract.
