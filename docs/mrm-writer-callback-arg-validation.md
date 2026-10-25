# Mrm writer callback-argument validation

`UrmCWRSetArgCallback` now rejects null descriptor-offset output storage and
propagates context and argument-binding failures before callback allocation and
after record growth. Valid callback descriptors retain their existing UID
serialization behavior.

The source-level regression check is
`tests/automated/check-mrm-writer-callback-arg-validation.py`.
