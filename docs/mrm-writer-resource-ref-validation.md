---
nav_exclude: true
---
# Mrm writer resource-reference validation

`UrmCWRSetArgResourceRef` now propagates context and argument-binding failures
before and after appending its resource descriptor.  Valid resource references
retain their existing serialization and argument layout.

The source-level regression is
`tests/automated/check-mrm-writer-resource-ref-validation.py`.
