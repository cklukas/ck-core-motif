# RowColumn accelerator text allocation

RowColumn converts virtual key bindings into human-readable accelerator text
using a growable buffer. Initial allocation and every resize are now checked,
and the old buffer remains valid when a resize cannot be satisfied. A zero
capacity is normalized before growth so the helper cannot loop forever.

The public accelerator conversion behavior is unchanged for successful
allocations; memory pressure now yields the existing partial-result fallback
instead of a null dereference. The source-level regression check is
`tests/automated/check-rowcolumn-key-buffer.py`.
