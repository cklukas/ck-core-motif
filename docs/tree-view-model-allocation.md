---
nav_exclude: true
---
# TreeView model allocation

`XmTreeViewSetNodes` now builds the replacement item model privately and
publishes it only after the item array and every copied label have been
allocated successfully.  An unavailable item array or label leaves the
existing tree, expansion state, and selection untouched; partial labels are
released before returning.

Successful node replacement and the public TreeView API are unchanged.  This
prevents allocation pressure from turning model updates into null dereferences
or partially replaced trees.
