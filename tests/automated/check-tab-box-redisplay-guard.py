#!/usr/bin/env python3
"""Check that XmTabBox redisplay uses its replacement GC after reconfiguration."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "TabBox.c").read_text()
start = source.index("static void\n#ifndef _NO_PROTO\nRedisplay(")
end = source.index("RedisplayTabs(XmTabBoxWidget", start)
redisplay = source[start:end]
assert "XtReleaseGC(widget, gc);" in redisplay
replacement = "XmTabBox__tab_GC(tab) = XmTabBox__text_GC(tab) = XtGetGC"
assert replacement in redisplay
replacement_index = redisplay.index(replacement)
local_gc = "gc = XmTabBox__tab_GC(tab);"
assert local_gc in redisplay[replacement_index:]
assert replacement_index < redisplay.index(local_gc, replacement_index)
print("PASS XmTabBox redisplay replacement-GC guard")
