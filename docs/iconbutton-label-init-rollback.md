# IconButton label initialization rollback

IconButton initialization now checks the private copy of an explicitly
provided `XmNlabelString`.  If copying the compound string fails, it rebuilds
the caption from `XmNlabel` or the widget name instead of leaving the button
with a NULL visible label.

The focused regression is
`tests/automated/check-iconbutton-label-init-rollback.py`.
