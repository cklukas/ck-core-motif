# Guard drag selection request lookups

Drag conversion callbacks can run while a selection request is being torn
down.  The incremental and normal drag callbacks now check the
`XtGetSelectionRequest` result before reading its timestamp, returning a
conversion failure when the request no longer exists.

The focused source regression is
`tests/automated/check-drag-selection-request-guard.py`.
