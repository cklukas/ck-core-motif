# MWM bindings property status handling

The startup binding loader reads a root-window property with
`XGetWindowProperty`. The result is usable only when that request returns
`Success`; output fields are not a substitute for the request status when the
server or property access fails. `GetBindingsProperty` now checks the status
before accepting the returned string and frees any returned data on failure.
