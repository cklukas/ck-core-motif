#!/usr/bin/env python3
"""Check cached bitmap entry validation for MWM labels."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeCachedLabelPixmap")
end = source.index("} /* END OF FUNCTION MakeCachedLabelPixmap", start)
body = source[start:end]

assert "bitmapc->bitmap == None" in body
assert "bitmapc->width == 0" in body
assert "bitmapc->height == 0" in body
assert "return ((Pixmap)NULL);" in body
assert body.index("bitmapc->bitmap == None") < body.index("XtGetValues")

print("PASS MWM cached-label entries are validated")
