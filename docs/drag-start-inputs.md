# Drag-start input contract

`XmDragStart` needs both a source widget and the initiating X event before it
can resolve the display and create a drag context.  It now returns `NULL` for
either missing input before application-context, display, or event-field
access.  Valid drag-start behavior is unchanged.
