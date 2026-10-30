# TabStack child-index initialization

`TabToIndex` now initializes its child index before scanning the composite
children.  Selecting a tab therefore passes a deterministic zero-based index
to the tab box instead of using an indeterminate local value.

The focused source-contract check is:

```sh
python3 tests/automated/check-tabstack-index-initialization.py
```
