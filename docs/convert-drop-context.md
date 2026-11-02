---
nav_exclude: true
---

# Motif-drop conversion context

The shared conversion handler hides the real selection atom for drag and drop
and obtains it from the active drag context.  It now rejects a Motif-drop
external conversion when that context has already been cleared, returning an empty
failure result before any `XtGetValues` or selection-request lookup.  This
keeps cancelled or failed drags from dereferencing a null context.

The regression check is `tests/automated/check-convert-drop-context.py`.

Local export-target negotiation is an intentional exception: XmeDragSource must
query targets before XmDragStart creates the context. The one-shot local flag is
consumed before early failures. The native [DNDDemo regression](dnd-demo.md)
checks this exception, retained rejection of external requests, and real drags.
