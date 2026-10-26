#!/usr/bin/env python3
"""Check source-pixmap validation in MWM icon creation."""

from pathlib import Path

source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeIconPixmap")
end = source.index("} /* END OF FUNCTION MakeIconPixmap", start)
body = source[start:end]

assert "bitmap == None || width == 0 || height == 0" in body
assert "return ((Pixmap)NULL);" in body
assert body.index("bitmap == None") < body.index("XCreatePixmap")

client_start = source.index("Pixmap MakeClientIconPixmap")
client_end = source.index("} /* END OF FUNCTION MakeClientIconPixmap", client_start)
client_body = source[client_start:client_end]
assert "pSD == NULL || iconBitmap == None" in client_body
assert client_body.index("iconBitmap == None") < client_body.index("XGetGeometry")

print("PASS MWM icon source geometry is validated")
