# Mrm callback fixup validation

`Urm__CW_FixupCallback` now validates callback descriptor metadata and its
record buffer before iterating items, rejects missing resource descriptors, and
skips callback installation when value conversion fails. Valid callback
records retain their existing resolution and unresolved-reference behavior.

The source-level regression check is
`tests/automated/check-mrm-callback-fixup-validation.py`.
