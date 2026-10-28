---
nav_exclude: true
---
# CommandPalette visible-index allocation

Filtering maintains a second array mapping visible List rows back to stored
commands. Its allocation now checks both the host `size_t` limit and Xt's
`Cardinal` allocation limit before calling `XtCalloc`. An unrepresentable item
count is rejected without changing the existing command model or callback ABI.

The source contract is checked by
`tests/automated/check-command-palette-visible-allocation.py`.
