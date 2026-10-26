# Text cursor-scroll line-table guard

`ScrollCursorVertically` now returns without touching the line table when the
table is unavailable or contains no lines. This covers initialization and
teardown windows where an action can arrive before layout storage is ready,
avoiding a null dereference or an out-of-range `line[number_lines]` access.

The focused source contract is checked by
`tests/automated/check-text-scroll-cursor-line-table.py`.
