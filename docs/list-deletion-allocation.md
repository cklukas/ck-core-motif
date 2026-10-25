# XmList deletion allocation failures

The list deletion helpers compact their arrays and then ask Xt to shrink the
allocation.  The previous code stored `XtRealloc`'s return value directly in
the widget.  A failed shrink therefore discarded the still-valid pointer and
left later list operations with a null or dangling array.

Each deletion path now computes the byte count with a checked `size_t`
conversion and commits the returned pointer only when it is non-null.  If a
shrink cannot be satisfied, the larger allocation is retained; the compacted
list and its public count remain valid, preserving existing Motif behavior.
