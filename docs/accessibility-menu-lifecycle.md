# Accessibility menu snapshots

Accessibility menu notifications snapshot child IDs before invoking providers
or transport callbacks. The snapshot allocation now checks the `Cardinal`
child count against `size_t` before multiplying by the ID size, preventing a
wrapped allocation on platforms where those widths differ.
