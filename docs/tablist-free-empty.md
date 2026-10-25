---
nav_exclude: true
---
# Empty tab-list cleanup

`XmTabListFree` now releases zero-count lists, including lists returned by the
tab-list widget constructor before their first tab is added. It returns early
before traversing the tab ring, preserving the normal cleanup path for
non-empty lists.
