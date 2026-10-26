"""Check Avatar initials use measured render-table text."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Avatar.c").read_text(
    encoding="utf-8"
)
redisplay = source[source.index("static void Redisplay(Widget widget") : source.index("static Boolean SetValues", source.index("static void Redisplay(Widget widget"))]
assert "XDrawString" not in redisplay
assert "XmStringExtent" in redisplay
assert "XmStringDraw" in redisplay
assert "XmNfontList" in source and "XmNrenderTable" in source
print("PASS Avatar initials use measured render-table text")
