# Tree child-operation list initialization

`XmTree` needs its child-operation list before constraint callbacks or destroy
processing can run.  If `_XmListInit()` cannot allocate that list, the
initializer now reports an Xt error and stops instead of returning a partially
initialized widget whose later callbacks would dereference a null list.

The focused source contract is checked by
`tests/automated/check-tree-initialize-list.py`.
