# Deterministic standard `WM_NAME` conversion

The standard Motif conversion for the `XmN_NAME` target walks the widget's
shell ancestors and reads `WM_NAME`.  A shell can legitimately have no such
property, or the property read can fail while a window is being destroyed.

The converter now initializes the X property metadata before the read and only
passes the text-property decoder a non-NULL value.  Missing names therefore
return an empty conversion instead of exposing uninitialized type/format data
to the decoder.  Existing successful `STRING`, compound-text, and UTF-8
conversion behavior is unchanged.

The source regression is `tests/automated/check-standard-convert-wm-name-init.py`.
