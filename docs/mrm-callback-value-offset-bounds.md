# Mrm callback-value offset bounds

`Urm__CW_FixupCallback` now validates non-immediate callback datum offsets
before converting them to pointers.  Integer, boolean, single-float, and null
values retain their immediate behavior; other values must point inside the
widget record.  Valid callback resource and literal values continue through
the existing conversion path.

The source-level regression is
`tests/automated/check-mrm-callback-value-offset-bounds.py`.
