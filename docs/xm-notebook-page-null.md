---
nav_exclude: true
---
# XmNotebook page-info null-safety

`XmNotebookGetPageInfo` now returns `XmPAGE_INVALID` for a null notebook or
output structure before app-context lookup. Valid page searches and output
contents remain unchanged.
