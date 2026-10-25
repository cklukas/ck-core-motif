---
nav_exclude: true
---
# File-selection directory cleanup

File-selection directory enumeration can reject an oversized allocation or a
failed `XmStringGenerate` conversion after entering its shared cleanup path.
The directory-list pointer now starts as `NULL`, so cleanup safely handles
those failures without reading an uninitialized stack value. Successful
directory listing behavior is unchanged.

The focused regression is
`tests/automated/check-file-selection-dir-cleanup.py`.
