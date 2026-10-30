---
nav_exclude: true
---
# Notebook page-number constraint guard

Automatic page-number allocation now skips null children and children without
constraint storage before reading their page numbers. Valid page allocation
is unchanged, while malformed composite state no longer reaches an unchecked
constraint dereference.

The source contract is checked by
`tests/automated/check-notebook-page-number-constraint-guard.py`.
