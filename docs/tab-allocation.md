---
nav_exclude: true
---
# Tab allocation guards

`XmTabCreate` and the internal `_XmTabCopy` helper now check both the tab
record and its optional decimal string allocation. A failed copy releases its
record and returns `NULL`; a failed resource-created tab is rejected before it
is linked into a tab list. Existing callers still receive the same tab data on
successful allocation, while allocation failures no longer become null
dereferences.
