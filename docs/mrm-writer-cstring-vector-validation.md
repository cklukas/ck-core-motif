# Mrm writer compound-string-vector validation

`UrmCWRSetArgCStringVec` now propagates context and argument-binding failures
before vector construction and after record growth.  Valid compound-string
vectors retain the existing size limits and UID serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-cstring-vector-validation.py`.
