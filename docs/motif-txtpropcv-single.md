# Single text-property result allocation

When an X text property produces exactly one string,
`TextPropertyToSingleTextItem` copies that item with `XtNewString`.  A failed
copy now releases the Xlib string list and returns `XNoMemory` instead of
reporting a successful conversion with a null result.

The regression check is
`tests/automated/check-txtpropcv-single-allocation.py`.
