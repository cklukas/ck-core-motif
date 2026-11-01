# MWM button-event callback dispatch

MWM stores window-manager functions behind the public `WmFunction` signature,
whose final argument is `XEvent *`. Button processing begins with the more
specific `XButtonEvent *`; it now explicitly converts that event storage at the
callback boundary. This preserves the event contents and callback ABI while
making the type contract clear to strict C compilers.

Run the focused check with:

```
python3 tests/automated/check-mwm-button-event-dispatch.py
```
