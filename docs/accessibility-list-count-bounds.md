---
nav_exclude: true
---
# Accessibility list count bounds

The accessibility bridge rebuilds a list's semantic child array when native
rows are inserted, removed, or reordered. The synchronizer now rejects
unrepresentable `count + custom` byte sizes before allocating or copying the
temporary array, and rejects an overflowing `old_count * 2` hash-table growth
calculation. This keeps malformed or hostile native counts fail-closed rather
than wrapping a `size_t` and corrupting the accessibility child state.

`tests/automated/check-accessible-list-count-bounds.py` is the focused source
regression for these guards.
