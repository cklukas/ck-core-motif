# UIL parent-list allocation failure handling

The recursive control-list traversal now checks parent-list node allocations
before linking them into a widget.  If bookkeeping allocation fails, the
current control-list member is skipped safely; the traversal does not
dereference a null node or corrupt the existing parent list.

The source-contract regression is
`tests/automated/check-uil-parent-list-allocation.py`.
