#!/usr/bin/env python3
"""Check failed MWM bitmap loads leave their reusable cache slot clean."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmImage.c").read_text(
    encoding="utf-8"
)
start = source.index("int GetBitmapIndex")
end = source.index("} /* END OF FUNCTION GetBitmapIndex */", start)
body = source[start:end]

read_failure = body.index("Unable to read bitmap file")
invalid_failure = body.index("Invalid bitmap file")
assert "bitmapc->path = NULL;" in body[read_failure:invalid_failure]
assert "bitmapc->bitmap = None;" in body[read_failure:invalid_failure]
assert "bitmapc->width = bitmapc->height = 0;" in body[read_failure:invalid_failure]
invalid_region = body[invalid_failure:body.index("else\n    /* builtin icon bitmap */", invalid_failure)]
assert "if (bitmapc->bitmap != None)" in invalid_region
assert "XFreePixmap (DISPLAY, bitmapc->bitmap);" in invalid_region
assert "bitmapc->path = NULL;" in invalid_region
assert "bitmapc->bitmap = None;" in invalid_region
print("PASS MWM failed bitmap loads clean the reusable cache slot")
