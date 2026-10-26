from pathlib import Path

source = Path("clients/mwm/WmGraphics.c").read_text(encoding="utf-8")
start = source.index("void WmInstallBitmapDataIntoXmCache")
end = source.index("END OF FUNCTION WmInstallBitmapDataIntoXmCache", start)
body = source[start:end]

assert "pchName == NULL || pData == NULL" in body
assert "width == 0 || height == 0" in body
assert body.index("pData == NULL") < body.index("XtMalloc")
print("PASS WSM raw-bitmap cache installation validates inputs")
