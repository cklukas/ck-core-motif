---
nav_exclude: true
---
# Accessibility menu snapshots

Accessibility menu notifications snapshot child IDs before invoking providers
or transport callbacks. The snapshot allocation checks the child count against
both `size_t` and Xt's `Cardinal` width before multiplying by the ID size,
preventing a wrapped allocation on platforms where those widths differ. The
accepted byte count is passed explicitly as an Xt `Cardinal`.


The snapshot allocation is also checked for failure before callback dispatch,
so an unavailable buffer leaves the notification path unchanged and avoids a
null dereference. The source regression is
`tests/automated/check-accessibility-menu-snapshot-allocation.py`.


The AT-SPI cache `GetItems` and initial tree-seeding snapshots also reject
unrepresentable count arithmetic. `GetItems` reports a D-Bus `NoMemory` error
when its ID array cannot be allocated, rather than dereferencing a null
buffer. The source regression is
`tests/automated/check-accessibility-cache-snapshot-allocation.py`.


Initial AT-SPI tree seeding now abandons the seeding pass if its ID snapshot
allocation fails, leaving the transport alive for later retries instead of
writing through a null buffer. The source regression is
`tests/automated/check-accessibility-seed-allocation.py`.
