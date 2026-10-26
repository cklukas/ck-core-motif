# Qualified-directory helper bounds

The internal `GetQualifiedDir` helper now retains path lengths as `size_t`
and uses one checked allocator for home-directory, absolute, and relative
path construction.  Overflow or an Xt `Cardinal`-unrepresentable result now
returns `NULL` instead of narrowing a length or allocating too little space.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmos-qualified-dir-helper-bounds.py
```
