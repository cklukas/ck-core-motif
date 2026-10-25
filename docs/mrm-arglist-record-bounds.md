# Mrm argument-list record bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now validate the complete
serialized argument-list descriptor before reading its fields.  They reject
negative or oversized counts and ensure every serialized argument descriptor
fits inside the widget record before the existing conversion and callback
logic runs.  Valid UID files retain the same argument and override behavior.

The source-level regression is
`tests/automated/check-mrm-arglist-record-bounds.py`.
