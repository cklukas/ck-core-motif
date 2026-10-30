---
nav_exclude: true
---
# Notebook page constraint guard

`XmNotebookGetPageInfo` now skips a missing child or child constraint record
while scanning page metadata. This keeps malformed composite state from being
dereferenced during lookup; valid Notebook page and tab metadata remains
unchanged.

The source contract is checked by
`tests/automated/check-notebook-page-constraint-guard.py`.
