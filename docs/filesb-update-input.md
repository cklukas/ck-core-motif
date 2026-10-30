---
nav_exclude: true
---

# FileSelectionBox update input

`FileSelectionBoxUpdate` now rejects missing dialog state or search data before
accessing list widgets and search callbacks. Normal directory and file search
updates are unchanged; malformed internal or embedder calls return safely.

The focused source contract is checked by
`tests/automated/check-filesb-update-input.py`.
