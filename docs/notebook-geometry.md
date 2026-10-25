---
nav_exclude: true
---
# Notebook geometry reply safety

`XmNotebook` recomputes a preferred size when a realized notebook changes its
layout resources. The preferred geometry reply is optional when the application
changes both dimensions at once, so `SetValues` now initializes the local
`XtWidgetGeometry` before applying the independent width and height decisions.
This preserves the existing resource precedence while keeping the code safe under
compiler data-flow analysis and future changes to the negotiation path.

The focused source check is
`tests/automated/check-notebook-geometry-init.py`. A native build should also
compile `lib/Xm/Notebook.c` with warnings enabled; the check is intended to keep
the initialization explicit in future maintenance.
