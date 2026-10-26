from pathlib import Path

source = Path("clients/mwm/WmGraphics.c").read_text(encoding="utf-8")
start = source.index("void WmInstallBitmapIntoXmCache")
end = source.index("END OF FUNCTION WmInstallBitmapIntoXmCache", start)
body = source[start:end]

assert "bitmap == None" in body
assert "width == 0 || height == 0" in body
assert "if (pImage == NULL)" in body
assert body.index("if (pImage == NULL)") < body.index("XmInstallImage")
print("PASS WSM bitmap-cache installation validates image resources")
