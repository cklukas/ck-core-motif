#!/usr/bin/env python3
"""Guard the ownership boundary for XmTabBox's mutable drawing context."""
from pathlib import Path
import re

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "TabBox.c").read_text()
match = re.search(r"static void\s+(?:#ifndef _NO_PROTO\s+)?Redisplay\(", source)
assert match is not None
start = match.start()
brace = source.index("{", start)
depth = 0
end = brace
while end < len(source):
    if source[end] == "{":
        depth += 1
    elif source[end] == "}":
        depth -= 1
        if depth == 0:
            end += 1
            break
    end += 1
redisplay = source[start:end]
assert "XtGetGC" not in redisplay and "XtReleaseGC" not in redisplay
assert "XFreeGC" not in redisplay
assert "XChangeGC" in redisplay and "XCreateGC" in redisplay
assert "sfield(font_list) == XmFontListCopy" not in source
print("PASS XmTabBox mutable-GC and font ownership guard")
