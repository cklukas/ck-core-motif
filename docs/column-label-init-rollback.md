---
nav_exclude: true
---
# Column constraint-label initialization rollback

Column now checks the private copy of a child constraint’s label string.  If
the copy fails, it installs an empty compound string (or NULL if that fallback
also fails), so the constraint never retains a borrowed resource pointer that
its destroy path could later free.

The focused regression is
`tests/automated/check-column-label-init-rollback.py`.
