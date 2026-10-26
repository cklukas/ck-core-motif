#!/usr/bin/env python3
"""Check null-client and missing-screen handling for named MWM icons."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeNamedIconPixmap")
end = source.index("} /* END OF FUNCTION MakeNamedIconPixmap", start)
body = source[start:end]

assert "if (pCD == NULL)" in body
assert "return ((Pixmap)NULL);" in body
assert "pSD = (pCD->pSD != NULL) ? pCD->pSD : wmGD.pActiveSD;" in body
assert "if (pSD == NULL)" in body
assert "if (iconName && pCD->pSD != NULL)" in body
assert body.index("if (pSD == NULL)") < body.index("pSD->builtinIconPixmap")

print("PASS MWM named-icon fallback rejects incomplete clients")
