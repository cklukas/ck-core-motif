---
nav_exclude: true
---

# FileSelectionBox list callback input

The FileSelectionBox list callback now ignores a missing callback or client
payload before accessing the selected item. It also leaves callback lengths at
zero when mask or directory compound-string construction fails, avoiding null
`XmStringLength` calls while preserving normal directory and file selection.

The focused source contract is checked by
`tests/automated/check-filesb-list-callback-input.py`.
