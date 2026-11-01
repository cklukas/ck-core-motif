# MWM icon-box event handlers

The icon-box event handlers now use Xt's native `XtEventHandler` function
signature directly: widget, client data, `XEvent`, and the dispatch-control
pointer.  This replaces incompatible function-pointer casts that could make
callback invocation undefined on ABI-sensitive platforms.  Key and focus
handlers select their concrete event view from the supplied `XEvent` only after
the normal null checks.

The focused source contract is covered by
`tests/automated/check-mwm-icon-event-handlers.py`.
