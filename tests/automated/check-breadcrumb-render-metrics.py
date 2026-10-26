from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/BreadcrumbBar.c").read_text(
    encoding="utf-8"
)
draw_item = source[source.index("static void draw_item"):]
draw_item = draw_item[:draw_item.index("static void Redisplay", 1)]
redisplay = source[source.index("static void Redisplay"):]

assert "XDrawString" not in redisplay
assert "separator_width + 4" in draw_item
assert "label_width + 4" in draw_item
assert "XmStringExtent(bar->breadcrumb.font" in source
assert "strlen(label)" not in source
assert "XmStringExtent" in redisplay
assert "XmStringDraw" in redisplay
print("PASS BreadcrumbBar uses measured separator, selection, and ellipsis geometry")
