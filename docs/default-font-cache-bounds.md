# Default font-list cache bounds

The per-display default render-table cache checks its eight-entry capacity
increment and resulting byte count before converting the allocation size to
Xt's `Cardinal` type. An unrepresentable cache growth returns failure without
changing the existing cache capacity.

The source-level regression is
`tests/automated/check-default-font-cache-bounds.py`.
