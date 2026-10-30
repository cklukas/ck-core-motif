# Drag-context construction failure handling

The drag-start path creates a private `XmDragContext` before registering the
source-destroy callback and starting the transfer. If Xt cannot create that
context, the path now releases its merged argument list, unlocks the display
state, and returns `NULL` without invoking callbacks or dereferencing an
invalid context.

The focused source-contract regression is
`tests/automated/check-drag-context-create-failure.py`.
