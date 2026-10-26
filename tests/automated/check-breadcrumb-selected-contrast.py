"""Check BreadcrumbBar selected labels remain readable."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BreadcrumbBar.c").read_text(
    encoding="utf-8"
)
draw = source[source.index("static void draw_item") : source.index("static void Redisplay", source.index("static void draw_item"))]
assert "XmStringDraw" in draw
assert "XSetForeground(display, bar->breadcrumb.gc, bar->core.background_pixel);" in draw
assert "XSetForeground(display, bar->breadcrumb.gc, bar->primitive.foreground);" in draw
assert "XmNfontList" in source and "XmNrenderTable" in source
print("PASS BreadcrumbBar selected labels use contrasting render-table text")
