---
nav_exclude: true
---
# Central conversion-handler inputs

`_XmConvertHandler` is the common entry point for clipboard, selection, and
drag conversion.  It now validates the widget, selection/target atoms, and
all output pointers before obtaining the display or looking up context state.
Direct and reentrant callers therefore receive a safe `False` result instead
of a null-pointer crash.

The focused regression is
`tests/automated/check-convert-handler-inputs.py`.
