---
nav_exclude: true
---
# SimpleSpinBox null-widget guards

`XmSimpleSpinBoxAddItem`, `XmSimpleSpinBoxDeletePos`, and
`XmSimpleSpinBoxSetItem` now return immediately when called with a null
widget. The guard runs before widget-part or application-context lookup, so
an invalid optional operation cannot crash the caller while valid widget
behavior remains unchanged.

The focused regression is
`tests/automated/check-spinbox-widget-inputs.py`.
