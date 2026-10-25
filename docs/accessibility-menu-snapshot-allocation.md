---
nav_exclude: true
---

# Accessibility menu snapshot allocation guard

Menu notifications snapshot child accessibility IDs before callbacks can
mutate the tree.  The snapshot now checks both host and Xt allocation limits
and returns safely when the ID array cannot be allocated.

Valid menu notifications are unchanged.  The static regression check is
`tests/automated/check-accessibility-menu-snapshot-allocation.py`.
