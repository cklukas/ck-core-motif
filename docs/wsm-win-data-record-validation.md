---
nav_exclude: true
---

# WSM window-data record validation

WSM message packing now validates every window-data record against the active
configuration format before calculating a message size or writing the wire
payload.  A scalar record must target a scalar 8-, 16-, or 32-bit attribute.
List records must match their declared element width and provide a non-null
payload whenever their length is nonzero.  Unknown record kinds, invalid
attribute widths, and non-empty null record arrays are rejected.

This preserves valid protocol messages while preventing a malformed caller
record from being sized as one type and packed as another, or from causing a
null payload dereference.

The focused regression is
`tests/automated/check-wsm-win-data-record-validation.py`.
