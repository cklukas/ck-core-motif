# Accessibility menu snapshots

Accessibility menu notifications snapshot child IDs before invoking providers
or transport callbacks. The snapshot allocation now checks the `Cardinal`
child count against `size_t` before multiplying by the ID size, preventing a
wrapped allocation on platforms where those widths differ.


The snapshot allocation is also checked for failure before callback dispatch,
so an unavailable buffer leaves the notification path unchanged and avoids a
null dereference. The source regression is
`tests/automated/check-accessibility-menu-snapshot-allocation.py`.
