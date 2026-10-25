#!/usr/bin/env python3
"""Check GC cache insertion failure releases the new X11 GC and entry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("if (!_XmAddHashEntry(gc_set")
end = source.index("\n      }", start) + len("\n      }")
failure = source[start:end]
assert "_XmProcessUnlock();" in failure
assert "XFreeGC(DisplayOfScreen(screen), gc_entry->gc);" in failure
assert "XtFree((char *)gc_entry);" in failure
assert "return (GC) NULL;" in failure
print("PASS GC cache insertion failure cleanup")
