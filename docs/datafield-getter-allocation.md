# DataField getter allocation safety

Legacy DataField diagnostics and getters now check every temporary allocation
before using it. This covers warning formatting, highlight initialization,
wide-string retrieval, selection retrieval, and baseline queries. On allocation
failure these paths return safely and leave the caller with a null or empty
result rather than dereferencing an uninitialized pointer.

The regression check is:

```sh
python3 tests/automated/check-datafield-getter-allocation.py
```
