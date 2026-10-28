# RowColumn baseline count

Baseline-aligned RowColumn children use the first or last value returned by
`XmWidgetGetBaselines`.  The query can produce an empty or null result, so
both RowColumn layout paths now require a positive count and a non-null array
before indexing it.  Empty results retain the existing zero-baseline fallback
instead of underflowing `line_count - 1` or dereferencing a null pointer.

The regression contract is checked by
`tests/automated/check-rclayout-baseline-count.py`.
