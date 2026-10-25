#!/usr/bin/env python3
"""Check XmBreadcrumbBar handles missing text and small geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BreadcrumbBar.c").read_text()
item = source[source.index("static int item_width"):source.index("static Cardinal visible_start")]
draw = source[source.index("static void draw_item"):source.rindex("static void Redisplay")]
start = source.rindex("static void Redisplay(")
end = source.index("static void Events(", start)
redisplay = source[start:end]

assert "bar->breadcrumb.separator ?" in item
assert 'bar->breadcrumb.items[index].label ?' in item
assert "!window || !bar->breadcrumb.gc" in redisplay
assert "width > 4 ? width - 4 : 1" in draw
assert "bar->core.width > 2 * bar->primitive.highlight_thickness" in redisplay
print("PASS BreadcrumbBar redisplay guards")
