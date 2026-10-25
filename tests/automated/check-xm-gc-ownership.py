#!/usr/bin/env python3
"""Check uncached image GCs are explicitly owned and released by the caller."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
assert "Boolean *caller_owns_gc" in source
call_start = source.index("gc = GetGCForPutImage(")
call_end = source.index("if (gc == NULL)", call_start)
assert "&caller_owns_gc" in source[call_start:call_end]
put_start = source.index("_XmPutScaledImage", call_end)
put_end = source.index("/* Destroy non-cached", put_start)
assert "if (caller_owns_gc)" in source[put_start:put_end]
assert "XFreeGC(display, gc);" in source[put_start:put_end]
helper = source[source.rindex("GetGCForPutImage(Screen *screen"):]
assert "*caller_owns_gc = True;" in helper
print("PASS uncached image GC ownership")
