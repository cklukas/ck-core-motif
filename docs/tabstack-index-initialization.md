# TabStack child-index initialization

`TabToIndex` now initializes its child index and bounds its scan by the
composite child count. Selecting a tab therefore passes a deterministic
zero-based index to the tab box without reading beyond Xt's child array.

The focused source-contract check is:

```sh
python3 tests/automated/check-tabstack-index-initialization.py
```
