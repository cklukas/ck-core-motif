from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/IconLabel.c").read_text(
    encoding="utf-8"
)
redisplay = source[source.index("static void Redisplay(Widget widget"):]
redisplay = redisplay[:redisplay.index("static Boolean SetValues", 1)]

assert "XDrawString" not in redisplay
assert "XmStringExtent" in redisplay
assert "XmStringDraw" in redisplay
assert "XmNfontList" in source
assert "XmNrenderTable" in source
assert "request->core.width" in source
print("PASS IconLabel uses render-table metrics and request-based natural geometry")
