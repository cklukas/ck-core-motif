---
nav_exclude: true
---
# Mrm writer char8-vector validation

`UrmCWRSetArgChar8Vec` now propagates context and argument-binding failures
before vector construction and after record growth.  Valid vectors retain the
existing size limits and UID serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-char8-vector-validation.py`.
