#!/usr/bin/env python3
"""Check TextOut coordinate hit testing validates source progress."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
prototype = source.index("XYToPos(XmTextWidget")
start = source.index("XYToPos(XmTextWidget", prototype + 1)
end = source.index("\nBoolean \n_XmTextShouldWordWrap", start)
body = source[start:end]

assert body.count("next_start <= start") >= 2
assert body.count("block.length > 0 && block.ptr == NULL") >= 2
assert body.count("return start;") >= 2

print("TextOut coordinate hit testing guards source progress")
