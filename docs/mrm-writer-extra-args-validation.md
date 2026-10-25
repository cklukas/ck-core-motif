---
nav_exclude: true
---
# Mrm writer extra-args validation

`UrmCWRSetExtraArgs` now propagates context validation and argument-descriptor
binding failures before writing the extra-argument count.  Valid records keep
the existing count limits and serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-extra-args-validation.py`.
