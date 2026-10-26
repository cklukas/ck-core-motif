from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Calendar.c").read_text(
    encoding="utf-8"
)
redisplay = source[source.index("static void Redisplay(Widget widget"):]
redisplay = redisplay[:redisplay.index("static Boolean month_allowed", 1)]

assert "XDrawString" not in redisplay
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmNfontList" in source
assert "XmNrenderTable" in source
print("PASS Calendar labels and day names use the measured render table")
