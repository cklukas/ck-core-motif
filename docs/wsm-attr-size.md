# WSM unsupported attribute sizes

WSM window-data unpacking now maps unsupported attribute widths to an inert
`WSM_NONE` record with zero length. This prevents malformed wire metadata from
leaving an undefined type that later dispatch or cleanup could interpret as a
valid list.

`tests/automated/check-wsm-attr-size.py` protects this malformed-input path.
