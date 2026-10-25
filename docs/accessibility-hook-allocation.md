---
nav_exclude: true
---

# Accessibility display-hook allocation guard

Accessibility watches display-level resource changes through a small hook
record.  If that record cannot be allocated, registration now returns without
installing a callback or dereferencing NULL.

Normal display-hook registration is unchanged.  The static regression check is
`tests/automated/check-accessibility-hook-allocation.py`.
