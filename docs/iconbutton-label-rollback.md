# IconButton label rollback

`XmIconButton` now copies changed label and label-string resources before
releasing their current values.  Allocation failure preserves the existing
label, while NULL remains an explicit clear value and the existing fallback
to the widget name remains intact.

The focused source regression is
`tests/automated/check-iconbutton-label-rollback.py`.
