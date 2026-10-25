# XIM registration lifecycle

XIM widget registration is optional state backed by Xt allocations.  If a
registration record or a widget-reference array cannot be allocated, the
input-method code now exits the current operation before dereferencing the
missing record or saving an Xlib context.  This keeps failed registration from
leaving a widget mapped to an XIC that has no corresponding lifetime reference.

The focused static regression is
`tests/automated/check-xmim-registration-lifecycle.py`.  It checks the
registration guards, the reference-growth failure check, and the destruction
callback path that walks saved widget state.
