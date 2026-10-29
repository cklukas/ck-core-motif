---
nav_exclude: true
---
# MultiList child allocation

`XmMultiList` creates its title, scrollbars, list frame, and optional search
controls as separate Xt children. The widget now tolerates an individual
creation failure: callbacks and geometry are installed only for children that
exist, list/frame sizing is skipped until its required children are present,
and search operations safely become no-ops when the search controls are
unavailable. Normal construction and layout are unchanged.

The contract is protected by
`tests/automated/check-multilist-child-allocation.py`.
