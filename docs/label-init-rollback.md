# Label initialization rollback

`XmLabel` now also checks the initial copy of a valid menu accelerator string.
If that copy cannot be allocated, the original valid accelerator remains in
place instead of being replaced by NULL.

The core Label widget now checks the private copy of a valid initial
`XmNlabelString`.  If copying fails, it rebuilds the caption from the widget
name instead of leaving a NULL label that later baseline and geometry code
could dereference.

The focused regression is
`tests/automated/check-label-init-rollback.py`.
