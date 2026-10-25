# XmList mutator null-safety

`XmListSetKbdItemPos`, `XmListSetHorizPos`, and `XmListSetAddMode` now return
safely when passed a null widget, before app-context locking or list-state access.
Valid list mutations retain their existing behavior.
