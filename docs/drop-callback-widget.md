---
nav_exclude: true
---

# Drop callback widget lifetime

Drop selection callbacks arrive with the drag context as their callback
widget.  Motif then rebinds the callback to the destination widget stored in
the transfer context before dispatching destination callbacks and completing
the transfer.  During destruction or cancellation that stored widget can be
cleared first.  `SelectionCallbackWrapper` now returns before using the
rebinding when `tc->widget` is null, preventing invalid `XtClass`/display
access during external-owner teardown.

The regression check is `tests/automated/check-drop-callback-widget.py`.
