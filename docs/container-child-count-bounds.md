# Container child-list count bounds

`XmContainerGetItemChildren` now checks the public `int` count before each
linked-list increment. An unrepresentably large or corrupted child list is
rejected instead of wrapping the count and allocating an undersized result
array.

The focused source contract is checked by
`tests/automated/check-container-child-count-bounds.py`.
