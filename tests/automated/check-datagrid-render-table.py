from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DataGrid.c").read_text(
    encoding="utf-8"
)
redisplay = source[source.index("static void Redisplay(Widget widget"):]

assert "XDrawString" not in redisplay
assert "XmStringExtent" in source
assert "XmStringDraw" in source
assert "XmNfontList" in source
assert "XmNrenderTable" in source
print("PASS DataGrid headers and cells use measured render-table text")
