# Exm Container detail-header copying

The educational Exm Container sample copies its `detailHeader` resource before
creating the associated IconHeader. That copy is now bounded by Xt's allocation
size, initialized before use, and transactional: a failed table or string copy
cleans up partial state and leaves the sample without an optional header rather
than dereferencing a failed allocation.

Run the focused source-contract check with:

```sh
python3 tests/automated/check-exm-container-header-copy.py
```
