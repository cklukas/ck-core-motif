---
nav_exclude: true
---
# Notebook widget-class guard

`XmNotebookGetPageInfo` now rejects `NULL` and non-Notebook widget handles
before looking up the application context or reading child constraints. A
malformed caller therefore receives `XmPAGE_INVALID` instead of risking a
read through an unrelated widget's private layout state. Valid Notebook page
queries are unchanged.

The source contract is checked by
`tests/automated/check-notebook-widget-class-guard.py`.
