# Workspace client-window C23 contracts

The Workspace demo's ICCCM client-window discovery helpers now use typed Xlib
declarations for display, window, atom, and property-result arguments.  The
recursive search continues to look for `WM_STATE` properties through child
windows, and the client-window module is accepted by the current C23 compiler
configuration.

The generated popup-menu post handler likewise declares the standard Xt event
handler arguments explicitly, so it can be registered directly with
`XtAddEventHandler` without an incompatible function pointer.

The focused source-contract check is:

```sh
python3 tests/automated/check-workspace-client-window-c23.py
```
