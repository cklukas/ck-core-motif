# Mrm argument-value offset bounds

`Urm__CW_CreateArglist` now validates non-immediate argument offsets before
converting them to pointers. Offsets must point inside the serialized widget
record; integer, boolean, single-float, and null representations retain their
immediate-value behavior. Invalid arguments are skipped through the existing
conversion-failure path, preserving valid UID argument ordering.

The source-level regression check is
`tests/automated/check-mrm-arg-value-offset-bounds.py`.
