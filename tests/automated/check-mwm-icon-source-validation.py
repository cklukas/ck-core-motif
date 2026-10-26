from pathlib import Path

source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeIconPixmap")
body = source[start:source.index("END OF FUNCTION MakeIconPixmap", start)]

assert "bitmap == None || width == 0 || height == 0" in body
assert body.index("width == 0") < body.index("XCreatePixmap")
print("PASS MWM icon conversion validates source bitmap")
