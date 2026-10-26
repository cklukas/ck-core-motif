#!/usr/bin/env python3
"""Guard the ownership boundary for XmTabBox's mutable drawing context."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "TabBox.c").read_text()
start = source.index("static void\n#ifndef _NO_PROTO\nRedisplay(")
end = source.index("RedisplayTabs(XmTabBoxWidget", start)
redisplay = source[start:end]
assert "XtGetGC" not in redisplay and "XtReleaseGC" not in redisplay
assert "XFreeGC" not in redisplay
assert "XChangeGC" in redisplay and "XCreateGC" in redisplay
assert "sfield(font_list) == XmFontListCopy" not in source
print("PASS XmTabBox mutable-GC and font ownership guard")
