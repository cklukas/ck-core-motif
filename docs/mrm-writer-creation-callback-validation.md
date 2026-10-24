# Mrm writer creation-callback validation

`UrmCWRSetCreationCallback` now rejects a null descriptor-offset output pointer
and propagates invalid-context failures before allocating or writing the
callback descriptor.  Valid creation-callback setup retains its existing
serialization and callback layout.

The source-level regression is
`tests/automated/check-mrm-writer-creation-callback-validation.py`.
