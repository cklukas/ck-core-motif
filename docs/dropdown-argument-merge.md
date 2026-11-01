---
nav_exclude: true
---
# DropDown argument-list merging

`XmDropDown` merges application resources with the internal resources used for
its text child, arrow, popup shell, and popup list.  The merged count is now
checked before addition, so a caller cannot wrap the legacy `Cardinal` count.
If allocation of a non-empty merged list fails, construction stops before an
Xt constructor receives a null argument list with a non-zero count.  A popup
whose list merge fails tears down its partial shell through the existing
destroy callback.

The focused source contract is covered by
`tests/automated/check-dropdown-argument-merge.py`.
