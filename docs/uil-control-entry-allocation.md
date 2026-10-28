# UIL control-entry allocation failure handling

The control-item and control-widget semantic actions now check their control
entry allocations before initialization or publishing the node into a list.
The control-widget action also restores the parse-frame order it temporarily
changes before returning on allocation failure, so an exhausted allocator
does not leave the parser with swapped frames.

The source-contract regression is
`tests/automated/check-uil-control-entry-allocation.py`.
