#!/usr/bin/env python3
"""Check compact-height protection in the MWM maximize glyph."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void BevelMaximizeButton (RList *prTop, RList *prBot, int x, int y,")
end = source.index("} /* END OF FUNCTION  BevelMaximizeButton", start)
body = source[start:end]

assert "if (height < 5)" in body
assert "return;" in body
assert body.index("if (height < 5)") < body.index("height-4")
assert body.index("if (height < 5)") < body.index("BevelRectangle")

print("PASS MWM maximize geometry rejects undersized heights")
