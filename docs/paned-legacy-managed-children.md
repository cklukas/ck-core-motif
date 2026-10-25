# Legacy PanedWindow managed-child growth

The legacy `XmPanedWidget` managed-child rebuild now grows its array through a
temporary pointer. Capacity addition and byte-size conversion are checked
against both `size_t` and Xt's `Cardinal` allocator width; failed growth leaves
the existing array and capacity unchanged and stops the rebuild safely.

The focused source regression is
`tests/automated/check-paned-legacy-managed-children.py`.
