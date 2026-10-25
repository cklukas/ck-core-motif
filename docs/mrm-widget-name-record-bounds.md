# Mrm widget-name record bounds

Mrm widget creation now validates serialized class and widget-name strings
before handing them to class lookup or the widget creator. Offsets must stay
inside the widget record and have a terminating NUL within the record. An
explicit override name remains supported without requiring the serialized name,
so existing override behavior is unchanged. Cleanup lookup applies the same
class-string validation before using the record.

The source-level regression check is
`tests/automated/check-mrm-widget-name-record-bounds.py`.
