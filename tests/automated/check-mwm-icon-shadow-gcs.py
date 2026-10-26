from pathlib import Path

source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeIconPixmap")
end = source.index("END OF FUNCTION MakeIconPixmap", start)
body = source[start:end]

assert "if (topGC == NULL || botGC == NULL)" in body
assert "XFreePixmap (DISPLAY, iconPixmap);" in body
assert body.index("topGC == NULL") < body.index("XFillRectangles")
print("PASS MWM icon shadow GC allocation is guarded")
