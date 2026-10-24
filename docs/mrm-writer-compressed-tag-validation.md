# Mrm writer compressed-tag validation

`UrmCWRSetCompressedArgTag` now propagates invalid-context and argument-binding
failures before writing the compressed tag into the UID record.  Valid tags and
related codes retain their existing serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-compressed-tag-validation.py`.
