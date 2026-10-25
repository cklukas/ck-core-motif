---
nav_exclude: true
---
# Column label initialization fallback

During `XmColumn` constraint initialization, a failed copy of a supplied
label string is replaced with an empty localized string. A null source remains
valid and produces no unnecessary copy. If generated-name allocation or the
implicit label child creation fails, initialization returns without marking a
label child as owned, so a later layout pass cannot dereference partial state.

The source contract is checked by
`tests/automated/check-column-label-allocation.py`.
