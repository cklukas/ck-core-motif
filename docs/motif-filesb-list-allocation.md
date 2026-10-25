---
nav_exclude: true
---
# FileSB directory-list allocation failure

File Selection Box directory searches convert each matching filename into an
`XmString` array before updating the file list.  The array allocation is now
checked before any entries are written.  If it fails, the temporary filename
list is released and the existing widget list remains unchanged while the
search callback completes safely.

The regression check is
`tests/automated/check-filesb-list-allocation.py`.
