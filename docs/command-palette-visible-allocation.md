---
nav_exclude: true
---
# CommandPalette visible-index allocation and stale-state guards

Filtering maintains a second array mapping visible List rows back to stored
commands. Its allocation now checks both the host `size_t` limit and Xt's
`Cardinal` allocation limit before calling `XtCalloc`. An unrepresentable item
count is rejected without changing the existing command model or callback ABI.

The palette now clears a released visible-index pointer before an oversized
request or allocation failure returns, so a later rebuild cannot free stale
storage. Invocation also verifies that a selected visible index still refers
to a current item. The public filter getter returns `NULL` if its SearchBox
child could not be constructed.

The source contract is checked by
`tests/automated/check-command-palette-visible-allocation.py`.
The related focused checks are
`tests/automated/check-command-palette-selection-bounds.py` and
`tests/automated/check-command-palette-filter-child-guard.py`.
