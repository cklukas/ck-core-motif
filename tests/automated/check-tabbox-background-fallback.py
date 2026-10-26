"""Check TabBox redisplay defaults to its own background resources."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text(
    encoding="utf-8"
)
start = source.index("Redisplay(Widget widget")
body = source[start : source.index("static void", start + 20)]
assert "Pixel            pixel = tab->core.background_pixel;" in body
assert "Pixmap           pixmap = tab->core.background_pixmap;" in body
print("PASS TabBox uses its own background when no TabStack supplies one")
