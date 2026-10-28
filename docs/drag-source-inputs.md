# Drag-source input guard

`XmeDragSource` now rejects a null initiating `XEvent` before it allocates a
conversion context or invokes the export-target converter.  `XmDragStart`
already requires a real button, key, or motion event; validating the same
precondition at the Motif transfer entry point avoids running user conversion
callbacks for a drag that cannot start.

Regression coverage: `tests/automated/check-drag-source-event-input.py`.
