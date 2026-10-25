# Mrm callback value-offset bounds

`Urm__CW_FixupCallback` now validates non-immediate callback value offsets
before evaluating them as pointers into the widget record. Integer, boolean,
single-float, and null callback values retain their immediate-value behavior;
malformed serialized offsets fail before conversion or resource lookup.

The source-level regression check is
`tests/automated/check-mrm-callback-value-offset-bounds.py`.
