---
nav_exclude: true
---
# Flyout

`XmFlyout` is an additive, non-modal anchored popup handle.  Create it as an
unmanaged child of its owner with `XmCreateFlyout`, configure
`XmNflyoutText`, `XmNflyoutOffsetX` and `XmNflyoutOffsetY`, and call
`XmFlyoutShow` or `XmFlyoutHide` explicitly.

Showing creates a transient override-redirect shell positioned below the owner
and clamped to the current screen.  The shell uses `XtGrabNone`: it never
changes menu or dialog admission and does not intercept unrelated events.
Owner unmap, owner button activation, Escape/Return in the popup, explicit
hide, owner destruction and flyout destruction all have deterministic cleanup.
`XmNflyoutDismissCallback` reports explicit or interaction-driven dismissal;
owner destruction does not call application callbacks after the owner is gone.
The flyout never moves keyboard focus.  Existing Motif tooltip resources and
behavior remain unchanged; delayed tooltips and teaching-tip content can be
layered on this primitive by a later package.

`tests/automated/run-flyout.py` validates show/hide, dismissal callbacks,
screen-edge placement through the installed prefix, and owner teardown under
Xvfb.
