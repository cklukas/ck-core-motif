# TabStack child-index initialization

`TabToIndex` and the optional `IndexToTab` helper now initialize their child
indices and bound scans by the composite child count. Selecting or looking up
a tab therefore uses deterministic zero-based indices without reading beyond
Xt's child array or dereferencing a NULL stack.

The focused source-contract check is:

```sh
python3 tests/automated/check-tabstack-index-initialization.py
```
