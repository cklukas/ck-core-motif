---
nav_exclude: true
---
# Dialog child lookup widget guards

`XmSelectionBoxGetChild` and `XmMessageBoxGetChild` now reject non-matching
widget classes before entering application context or reading private child
fields. Valid dialog child identifiers and returned children are unchanged.

The source contract is checked by
`tests/automated/check-dialog-child-widget-class.py`.
