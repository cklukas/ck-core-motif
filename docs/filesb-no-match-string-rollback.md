# FileSelectionBox no-match string rollback

`XmFileSelectionBox` now copies a replacement `XmNnoMatchString` before
freeing the current widget-owned value.  If the copy cannot be completed, the
existing message remains installed rather than leaving the dialog with a null
or dangling message string.

The focused regression is
`tests/automated/check-filesb-no-match-string-rollback.py`.
