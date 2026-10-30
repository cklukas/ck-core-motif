---
nav_exclude: true
---
# Container end-select focus guards

When finishing selection, Container now treats a focused child without a
constraint record as outside the selected set instead of dereferencing null
state.  Focus restoration and valid selection behavior remain unchanged.

The source contract is checked by
`tests/automated/check-container-end-select-focus-guards.py`.
