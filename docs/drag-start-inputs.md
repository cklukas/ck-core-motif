# Drag-start input contract

`XmDragStart` needs both a source widget and the initiating X event before it
can resolve the display and create a drag context.  It now returns `NULL` for
either missing input, or for a nonzero argument count with no argument list,
before application-context, display, or event-field access.  Valid drag-start
behavior is unchanged.
The maximum representable argument count is also rejected because the drag
context creation path appends one internal argument.
