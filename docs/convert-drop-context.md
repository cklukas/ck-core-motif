---
nav_exclude: true
---

# Motif-drop conversion context

The shared conversion handler hides the real selection atom for drag and drop
and obtains it from the active drag context.  It now rejects a Motif-drop
conversion when that context has already been cleared, returning an empty
failure result before any `XtGetValues` or selection-request lookup.  This
keeps cancelled or failed drags from dereferencing a null context.

The regression check is `tests/automated/check-convert-drop-context.py`.
