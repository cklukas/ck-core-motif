# UIL nested-list allocation failure handling

Both nested-list entry paths now check their semantic node allocation before
initialization or forward-reference patching.  If a nested-list wrapper cannot
be allocated, the list is left unchanged and the parser action returns without
dereferencing a null wrapper.

The source-contract regression is
`tests/automated/check-uil-nested-list-allocation.py`.
