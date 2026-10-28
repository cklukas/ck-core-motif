---
nav_exclude: true
---
# DataGrid top-row bounds

DataGrid keyboard and wheel navigation now saturate `top_row` before passing it
to the existing viewport clamp. This prevents Page Down or wheel-down from
wrapping a large `Cardinal` row index back to the beginning. Selection-driven
visibility checks also subtract only after proving the row is at or below the
current top, avoiding an overflowing sum while preserving normal scrolling.

The source contract is checked by
`tests/automated/check-datagrid-top-row-overflow.py`.
