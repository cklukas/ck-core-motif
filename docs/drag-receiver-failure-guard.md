# Guard drag motion after receiver allocation failure

If receiver-info allocation fails while the pointer crosses into a new
window, the drag motion protocol now stops that update before accessing the
missing receiver record.  This keeps allocation pressure from turning a
recoverable discovery failure into a null dereference; later motion events can
retry discovery normally.

The focused source regression is
`tests/automated/check-drag-receiver-failure-guard.py`.
