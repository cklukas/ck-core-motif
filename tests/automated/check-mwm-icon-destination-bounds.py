#!/usr/bin/env python3
"""Check destination-size bounds in MWM icon creation."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeIconPixmap")
end = source.index("} /* END OF FUNCTION MakeIconPixmap", start)
body = source[start:end]

assert "iconImageMaximum.width == 0" in body
assert "iconImageMaximum.height == 0" in body
assert "UINT_MAX -" in body
assert body.index("iconImageMaximum.width == 0") < body.index("imageWidth =")
assert body.index("UINT_MAX -") < body.index("imageHeight =")

print("PASS MWM icon destination dimensions are bounded")
