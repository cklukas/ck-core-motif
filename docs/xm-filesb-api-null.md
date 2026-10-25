# XmFileSelectionBox API null-safety

`XmFileSelectionBoxGetChild` now returns `NULL` for a null widget, and
`XmFileSelectionDoSearch` returns before app-context access for a null widget.
Valid child lookup and search behavior remain unchanged.
