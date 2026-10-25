# List drag conversion allocation

List drag initiation now checks the conversion record and temporary string-array
allocations before publishing `drag_conv`. Allocation failure cancels that drag
attempt and clears the widget pointer, leaving later drag attempts consistent.

The source-level regression is
`tests/automated/check-list-drag-allocation.py`.
