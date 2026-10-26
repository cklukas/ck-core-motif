# Qualified directory length bounds

`GetQualifiedDir` now keeps `strlen` results in `size_t` until the input has
been checked against the `int`, host-size, and Xt `Cardinal` limits.  This
prevents narrowing and allocation-size wraparound when callers provide very
long directory or filter strings, while preserving ordinary path expansion.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmos-qualified-dir-bounds.py
```
