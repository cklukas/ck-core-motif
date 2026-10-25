#!/usr/bin/env python3
"""Check VendorS secondary-object setup has balanced process locking."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c").read_text()
start = source.index("pePtr = _XmGetBaseClassExtPtr")
end = source.index("extData =", start)
window = source[start:end]

assert window.count("_XmProcessLock();") == 0
assert window.count("_XmProcessUnlock();") == 1
assert "size = vec->core_class.widget_size;" in window
assert "newSec = XtMalloc(size);" in window

print("VendorS secondary-object lock scope is balanced")
