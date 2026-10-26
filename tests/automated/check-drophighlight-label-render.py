"""Check DropHighlight renders its public label with centered metrics."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropHighlight.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Redisplay(Widget widget")
redisplay = source[start : source.index("static Boolean SetValues", start)]
assert "XmStringExtent" in redisplay
assert "XmStringDraw" in redisplay
assert "label_x = ((int)highlight->core.width - (int)label_width) / 2;" in redisplay
assert "label_y = ((int)highlight->core.height - (int)label_height) / 2;" in redisplay
assert "XmNfontList" in source and "XmNrenderTable" in source
print("PASS DropHighlight draws centered labels through its render table")
