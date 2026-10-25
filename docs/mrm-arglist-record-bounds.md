# Mrm serialized argument-list bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now validate the complete
serialized argument-list descriptor before iterating its argument records. The
descriptor header must fit in the widget record, counts must be non-negative
and within the existing list limit, and the serialized argument vector must fit
in the remaining record bytes. Malformed UID records therefore fail before a
truncated descriptor can drive an out-of-bounds read, while valid records keep
the existing argument conversion behavior.

The source-level regression check is
`tests/automated/check-mrm-arglist-record-bounds.py`.
