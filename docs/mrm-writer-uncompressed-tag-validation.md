# Mrm writer uncompressed-tag validation

`UrmCWRSetUncompressedArgTag` now propagates context and argument-binding
failures both before and after appending the tag string.  Valid tags retain
their existing serialization and argument layout.

The source-level regression is
`tests/automated/check-mrm-writer-uncompressed-tag-validation.py`.
