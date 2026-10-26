# Drag-cancel input contract

`XmDragCancel` terminates an active drag context and sends its completion
state through the normal finish path.  A null drag-context widget has no
context to cancel; the function now returns before application-context lookup
or casting.  Valid drag cancellation is unchanged.
