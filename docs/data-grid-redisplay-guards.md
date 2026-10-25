# DataGrid redisplay guards

`XmDataGrid` now skips drawing safely when its GC is unavailable, treats null
cell text as an empty string, and clamps selected-cell rectangle dimensions for
narrow columns or short rows. This avoids null-handle Xlib calls, null-string
length faults, and unsigned geometry underflow while preserving normal grids.

The focused source contract is checked by
`tests/automated/check-data-grid-redisplay-guards.py`.
