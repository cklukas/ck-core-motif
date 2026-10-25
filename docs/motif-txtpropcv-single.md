# Single text-property copy failure

When Xlib returns one text item, `TextPropertyToSingleTextItem` now checks the
`XtNewString` copy. Failure releases the Xlib text list and returns
`XNoMemory` instead of exposing a null result.

The regression check is
`tests/automated/check-txtpropcv-single-allocation.py`.
