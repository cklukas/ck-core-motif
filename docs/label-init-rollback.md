# Label initialization rollback

The core Label widget now checks the private copy of a valid initial
`XmNlabelString`.  If copying fails, it rebuilds the caption from the widget
name instead of leaving a NULL label that later baseline and geometry code
could dereference.

The focused regression is
`tests/automated/check-label-init-rollback.py`.
