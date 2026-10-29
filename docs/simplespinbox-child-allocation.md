---
nav_exclude: true
---
# SimpleSpinBox child allocation

`XmSimpleSpinBox` creates its text-field child during initialization. If Xt
cannot allocate that child, initialization now releases the temporary name and
returns without querying a null widget. The public add, delete, and set-item
APIs treat the incomplete control as a no-op until a text child exists, while
normal spin-box behavior is unchanged.

The contract is protected by
`tests/automated/check-simplespinbox-child-allocation.py`.
