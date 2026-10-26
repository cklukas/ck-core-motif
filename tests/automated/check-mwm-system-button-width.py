#!/usr/bin/env python3
"""Check narrow-width protection in the MWM system glyph."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void BevelSystemButton")
end = source.index("} /* END OF FUNCTION  BevelSystemButton", start)
body = source[start:end]

assert "height >= 8 && height <= 9 && width < 4" in body
assert "height >= 10 && height <= 13 && width < 6" in body
assert "height >= 14 && width < 8" in body
guard = body.index("if ((height >= 8")
assert guard < body.index("width - 4")
assert guard < body.index("width - 6")
assert guard < body.index("width - 8")

print("PASS MWM system-button width is validated before subtraction")
