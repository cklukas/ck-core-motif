#!/usr/bin/env python3
"""Check null-screen handling in MWM icon creation."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeIconPixmap")
end = source.index("} /* END OF FUNCTION MakeIconPixmap", start)
body = source[start:end]

assert "if (pSD == NULL)" in body
assert "return ((Pixmap)NULL);" in body
assert body.index("if (pSD == NULL)") < body.index("pSD->iconImageMinimum")

print("PASS MWM icon creation rejects missing screen records")
