---
nav_exclude: true
---

# DataField primary-selection request allocation

The primary-selection request path now checks its `_XmTextPrimSelect`
allocation before initializing the request state. If allocation fails, the
selection target result and temporary action/event records are released before
returning, preventing both a null dereference and a leaked request snapshot.

The focused source contract is checked by
`tests/automated/check-datafield-primary-selection.py`.
