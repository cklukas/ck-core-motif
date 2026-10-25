# Scrolled-window auto-drag lifetime

`XmScrolledWindow` auto-drag uses an Xt timeout to move the active scrollbar
while a drop remains over an edge region.  The timeout payload is owned by the
scrolled window, not by the scrollbar, and is retired on every exit path:

- drop-site leave;
- disabling `XmNautoDragModel`;
- scrollbar removal or destruction;
- scrolled-window destruction; and
- a value-change callback that destroys or mutates either widget.

The timeout callback marks itself active while it calls the scroll-frame
navigator protocol.  It re-checks cancellation after that protocol returns,
because application callbacks can destroy the scrollbar or its parent during
the notification.  Destroy callbacks detach the payload before Xt completes
the child destruction walk; idle payload storage is released by Xt after the
walk has unwound.  No callback path dereferences a scrollbar after its
destruction notification.

The scrollbar navigator dispatcher also snapshots its callback records before
calling application code.  This keeps Xt from iterating a callback list that a
reentrant callback has caused to be freed.  The scroll-frame navigator loop
stops when a callback marks the frame for destruction rather than visiting a
borrowed navigator list after its owner has retired it.

The native lifetime regression exercises destruction, disable, leave,
repeated-timer, and callback-triggered mutation scenarios.  Run it against an
installed build under Xvfb; repeat the process to cover allocator and event
ordering differences.  Sanitizer builds should run the same scenario set.
