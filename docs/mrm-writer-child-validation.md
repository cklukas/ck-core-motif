# Mrm writer child validation

`UrmCWRSetChild` now propagates invalid-context failures before reading the
widget record or child descriptor. Valid child indexes, resource IDs, and
management flags retain their existing UID serialization behavior.

The source-level regression check is
`tests/automated/check-mrm-writer-child-validation.py`.
