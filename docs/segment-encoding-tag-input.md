# Segment-encoding tag input

`XmRegisterSegmentEncoding` and `XmMapSegmentEncoding` now reject a null font
list element tag before looking up the encoding registry. This avoids a null
`strcmp` input while preserving valid lookups, registrations, and the existing
`NULL` encoding value used to unregister a valid tag.

Regression coverage: `tests/automated/check-segment-encoding-tag-input.py`.
