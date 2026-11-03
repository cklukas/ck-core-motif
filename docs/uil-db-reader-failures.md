---
nav_exclude: true
---
# UIL database reader failures

The compound WMD table readers now return success only after consuming their
entire record.  The outer loader closes the database immediately when a helper
rejects a count, cannot allocate, or encounters a short read.  It therefore
cannot interpret unread record bytes as another database header.

Zero-byte table payloads are handled without calling `fread` with an empty
element size.  This preserves valid empty records while making incomplete
records fail deterministically.

`tests/automated/check-uil-db-reader-status.py` protects the reader/loader
contract.
