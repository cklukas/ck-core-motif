---
nav_exclude: true
---
# Drag conversion callback inputs

The drag conversion wrapper is called by Xt while a drag context is being
created, cancelled, or torn down.  It now validates the context and all
conversion output pointers before obtaining the display, and refuses a
conversion whose originating widget has not been installed in the context
block.  This makes reentrant and cancellation paths fail safely instead of
dereferencing incomplete callback state.

The focused regression is `tests/automated/check-drag-convert-inputs.py`.
