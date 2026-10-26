from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DatePicker.c").read_text(
    encoding="utf-8"
)
redisplay = source[source.index("static void Redisplay(Widget widget"):]
redisplay = redisplay[:redisplay.index("static void Events", 1)]

assert "XDrawString" not in redisplay
assert "XmStringExtent" in redisplay
assert "XmStringDraw" in redisplay
assert "XmNfontList" in source
assert "XmNrenderTable" in source
print("PASS DatePicker draws its date through measured render-table text")
