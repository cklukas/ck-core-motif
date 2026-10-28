# UIL forward-list allocation failure handling

The forward-referenced nested-list action now checks its wrapper allocation
before installing the list patch or linking the wrapper into the list.  On
failure, the list and forward-reference chain remain untouched and the parser
action returns safely.

The source-contract regression is
`tests/automated/check-uil-forward-list-allocation.py`.
