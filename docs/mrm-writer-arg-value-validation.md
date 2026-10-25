---
nav_exclude: true
---
# Mrm writer arg-value validation

`UrmCWRSetArgValue` now propagates invalid-context and argument-binding failures
before writing the argument representation type. Valid values retain their
existing type-specific serialization and conversion behavior.

The source-level regression is
`tests/automated/check-mrm-writer-arg-value-validation.py`.
