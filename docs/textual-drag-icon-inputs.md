# Textual drag-icon lookup input contract

`XmeGetTextualDragIcon` derives a screen, display, and root-window context
from its widget before creating or reusing the textual drag icon.  A null
widget has none of those resources; the function now returns `NULL` before
`XtScreen` or application-context lookup.  Valid-widget behavior is
unchanged.
