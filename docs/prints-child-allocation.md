# PrintS child descriptor allocation

The child side of `XmPrintToFile` creates a descriptor before opening its
application display.  The descriptor and copied file name are now checked;
allocation failure releases any partial state and exits the child cleanly
instead of dereferencing a null descriptor or passing a null name into the
printing callbacks.

The regression check is `tests/automated/check-prints-child-allocation.py`.
