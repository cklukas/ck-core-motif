# Font selector size-list allocation

Font selector size and alternate-family list refreshes now check their
temporary `XmString` arrays for overflow and allocation failure before filling
them. A failure returns without updating the corresponding list, preventing
null-pointer writes while preserving normal behavior for valid lists.

The source-level regression check is
`tests/automated/check-fonts-size-list-allocation.py`.
