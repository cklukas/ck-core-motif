#!/usr/bin/env python3
"""Check that DT session-hint processing rejects a null screen record."""

from pathlib import Path


source = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")
start = source.index("void GetDtSessionHints (WmScreenData *pSD, int sNum)")
end = source.index("} /* END OF FUNCTION GetDtSessionHints */", start)
body = source[start:end]

assert "if (pSD == NULL)" in body
assert "return;" in body
assert body.index("if (pSD == NULL)") < body.index("pSD->rootWindow")

print("PASS MWM DT session-hint processing rejects null screen data")
