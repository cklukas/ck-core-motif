from pathlib import Path

source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeNamedIconPixmap")
end = source.index("END OF FUNCTION MakeNamedIconPixmap", start)
body = source[start:end]

assert "if (XmeGetPixmapData" in body
assert "pixmap = XmUNSPECIFIED_PIXMAP;" in body
assert body.index("if (XmeGetPixmapData") < body.index("MakeIconPixmap")
print("PASS MWM named-icon metadata failure falls back safely")
